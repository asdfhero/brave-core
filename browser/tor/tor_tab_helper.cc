/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/tor/tor_tab_helper.h"

#include <utility>

#include "brave/browser/profiles/profile_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/navigation_handle.h"

namespace tor {

const int kMaxRetryLimit = 7;

TorTabHelper::TorTabHelper(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents), retry_count_(0) {}

TorTabHelper::~TorTabHelper() = default;

// static
void TorTabHelper::MaybeCreateForWebContents(
    content::WebContents* web_contents) {
  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  if (!brave::IsTorProfile(profile))
    return;
  TorTabHelper::CreateForWebContents(web_contents);
}

void TorTabHelper::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  if (navigation_handle->GetNetErrorCode() !=
      net::ERR_PROXY_CONNECTION_FAILED)
    return;
  if (retry_count_ < kMaxRetryLimit) {
    VLOG(1) << "Tor proxy error retrying";
    content::OpenURLParams params =
        content::OpenURLParams::FromNavigationHandle(navigation_handle);
    navigation_handle->GetWebContents()->OpenURL(std::move(params));
    ++retry_count_;
  } else {
    VLOG(1) << "Tor proxy error max retries reached";
    // New error page
  }
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(TorTabHelper)

}  // namespace tor
