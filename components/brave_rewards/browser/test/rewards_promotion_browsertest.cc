/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_rewards/rewards_service_factory.h"
#include "brave/common/brave_paths.h"
#include "brave/components/brave_rewards/browser/rewards_service_impl.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_context_helper.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_context_util.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_network_util.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_promotion.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_response.h"
#include "brave/components/brave_rewards/browser/test/common/rewards_browsertest_util.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/testing_profile.h"
#include "content/public/test/browser_test.h"
#include "net/dns/mock_host_resolver.h"

// npm run test -- brave_browser_tests --filter=RewardsPromotionBrowserTest.*

namespace rewards_browsertest {

class RewardsPromotionBrowserTest : public InProcessBrowserTest {
 public:
  RewardsPromotionBrowserTest() {
    promotion_ = std::make_unique<RewardsBrowserTestPromotion>();
    response_ = std::make_unique<RewardsBrowserTestResponse>();
  }

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    // HTTP resolver
    https_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));
    https_server_->SetSSLConfig(net::EmbeddedTestServer::CERT_OK);
    https_server_->RegisterRequestHandler(
        base::BindRepeating(&rewards_browsertest_util::HandleRequest));
    ASSERT_TRUE(https_server_->Start());

    // Rewards service
    brave::RegisterPathProvider();
    auto* profile = browser()->profile();
    rewards_service_ = static_cast<brave_rewards::RewardsServiceImpl*>(
        brave_rewards::RewardsServiceFactory::GetForProfile(profile));

    // Response mock
    base::ScopedAllowBlockingForTesting allow_blocking;
    response_->LoadMocks();
    rewards_service_->ForTestingSetTestResponseCallback(
        base::BindRepeating(
            &RewardsPromotionBrowserTest::GetTestResponse,
            base::Unretained(this)));
    rewards_service_->SetLedgerEnvForTesting();

    // Other
    promotion_->Initialize(browser(), rewards_service_);
  }

  void TearDown() override {
    InProcessBrowserTest::TearDown();
  }

  void GetTestResponse(
      const std::string& url,
      int32_t method,
      int* response_status_code,
      std::string* response,
      std::map<std::string, std::string>* headers) {
    response_->Get(
        url,
        method,
        response_status_code,
        response);
  }

  double ClaimPromotion(bool use_panel) {
    // Wait for promotion to initialize
    promotion_->WaitForPromotionInitialization();

    // Use the appropriate WebContents
    content::WebContents* contents = use_panel
        ? rewards_browsertest_helper::OpenRewardsPopup(browser())
        : browser()->tab_strip_model()->GetActiveWebContents();

    // Claim promotion via settings page or panel, as instructed
    if (use_panel) {
      rewards_browsertest_util::WaitForElementThenClick(
          contents,
          "button");
    } else {
      rewards_browsertest_util::WaitForElementThenClick(
          contents,
          "[data-test-id='claimGrant']");
    }

    // Wait for CAPTCHA
    rewards_browsertest_util::WaitForElementToAppear(
        contents,
        "[data-test-id='captcha']");

    rewards_browsertest_util::DragAndDrop(
        contents,
        "[data-test-id=\"captcha-triangle\"]",
        "[data-test-id=\"captcha-drop\"]");

    promotion_->WaitForPromotionFinished();

    // Ensure that promotion looks as expected
    auto promotion = promotion_->GetPromotion();
    EXPECT_STREQ(std::to_string(promotion.amount).c_str(), "30.000000");
    EXPECT_STREQ(
        promotion.promotion_id.c_str(),
        promotion_->GetPromotionId().c_str());
    EXPECT_EQ(promotion.type, 0u);
    EXPECT_EQ(promotion.expires_at, 1740816427ull);

    // Check that promotion notification shows the appropriate amount
    const std::string selector = use_panel
        ? "[id='root']"
        : "[data-test-id='newTokenGrant']";
    rewards_browsertest_util::WaitForElementToContain(
        contents,
        selector,
        "Free Token Grant");
    rewards_browsertest_util::WaitForElementToContain(
        contents,
        selector,
        "30.000 BAT");

    // Dismiss the promotion notification
    if (use_panel) {
      rewards_browsertest_util::WaitForElementThenClick(
          contents,
          "#"
          "grant-completed-ok");
    }

    return 30;
  }

  brave_rewards::RewardsServiceImpl* rewards_service_;
  std::unique_ptr<net::EmbeddedTestServer> https_server_;
  std::unique_ptr<RewardsBrowserTestPromotion> promotion_;
  std::unique_ptr<RewardsBrowserTestResponse> response_;
};

IN_PROC_BROWSER_TEST_F(RewardsPromotionBrowserTest, ClaimViaSettingsPage) {
  rewards_browsertest_helper::EnableRewards(browser());

  double balance = ClaimPromotion(false);
  ASSERT_EQ(balance, 30.0);
}

IN_PROC_BROWSER_TEST_F(RewardsPromotionBrowserTest, ClaimViaPanel) {
  rewards_browsertest_util::EnableRewardsViaCode(browser(), rewards_service_);

  double balance = ClaimPromotion(true);
  ASSERT_EQ(balance, 30.0);
}

IN_PROC_BROWSER_TEST_F(
    RewardsPromotionBrowserTest,
    PromotionHasEmptyPublicKey) {
  response_->SetPromotionEmptyKey(true);
  rewards_browsertest_helper::EnableRewards(browser());

  promotion_->WaitForPromotionInitialization();
  rewards_browsertest_util::WaitForElementToAppear(
      rewards_browsertest_helper::OpenRewardsPopup(browser()),
      "[data-test-id=notification-close]",
      false);
}

}  // namespace rewards_browsertest
