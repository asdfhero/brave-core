/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/wallet/wallet_balance.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/json/json_reader.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_number_conversions.h"
#include "bat/ledger/global_constants.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/uphold/uphold.h"
#include "bat/ledger/internal/request/request_promotion.h"
#include "bat/ledger/internal/static_values.h"
#include "bat/ledger/internal/state/state_util.h"
#include "net/http/http_status_code.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace braveledger_wallet {

WalletBalance::WalletBalance(bat_ledger::LedgerImpl* ledger) :
    uphold_(std::make_unique<braveledger_uphold::Uphold>(ledger)),
    ledger_(ledger) {
}

WalletBalance::~WalletBalance() = default;

void WalletBalance::Fetch(ledger::FetchBalanceCallback callback) {
  // if we don't have user funds in anon card anymore
  // we can skip balance server ping
  if (!braveledger_state::GetFetchOldBalanceEnabled(ledger_)) {
    auto balance = ledger::Balance::New();
    balance->user_funds = "0";
    GetUnblindedTokens(std::move(balance), callback);
    return;
  }

  const std::string payment_id = braveledger_state::GetPaymentId(ledger_);

  const std::string url = braveledger_request_util::GetBalanceWalletURL(
      braveledger_state::GetPaymentId(ledger_));

  auto load_callback = std::bind(&WalletBalance::OnFetch,
      this,
      _1,
      callback);
  ledger_->LoadURL(url, {}, "", "", ledger::UrlMethod::GET, load_callback);
}

void WalletBalance::OnFetch(
    const ledger::UrlResponse& response,
    ledger::FetchBalanceCallback callback) {
  BLOG(6, ledger::UrlResponseToString(__func__, response));

  ledger::BalancePtr balance = ledger::Balance::New();
  if (response.status_code != net::HTTP_OK) {
    callback(ledger::Result::LEDGER_ERROR, std::move(balance));
    return;
  }

  base::Optional<base::Value> value = base::JSONReader::Read(response.body);
  if (!value || !value->is_dict()) {
    BLOG(0, "Response is not JSON");
    callback(ledger::Result::LEDGER_ERROR, std::move(balance));
    return;
  }

  base::DictionaryValue* dictionary = nullptr;
  if (!value->GetAsDictionary(&dictionary)) {
    BLOG(0, "Response is not JSON");
    callback(ledger::Result::LEDGER_ERROR, std::move(balance));
    return;
  }

  const auto confirmed = dictionary->FindDoubleKey("confirmed");
  if (confirmed) {
    balance->total = *confirmed;
  }

  balance->user_funds = balance->total;
  balance->wallets.insert(
      std::make_pair(ledger::kWalletAnonymous, balance->total));

  if (balance->total == 0.0) {
    braveledger_state::SetFetchOldBalanceEnabled(ledger_, false);
  }

  GetUnblindedTokens(std::move(balance), callback);
}

void WalletBalance::GetUnblindedTokens(
    ledger::BalancePtr balance,
    ledger::FetchBalanceCallback callback) {
  if (!balance) {
    BLOG(0, "Balance is null");
    callback(ledger::Result::LEDGER_ERROR, std::move(balance));
    return;
  }

  auto tokens_callback = std::bind(&WalletBalance::OnGetUnblindedTokens,
      this,
      *balance,
      callback,
      _1);
  ledger_->GetSpendableUnblindedTokensByBatchTypes(
      {ledger::CredsBatchType::PROMOTION},
      tokens_callback);
}

void WalletBalance::OnGetUnblindedTokens(
    ledger::Balance info,
    ledger::FetchBalanceCallback callback,
    ledger::UnblindedTokenList list) {
  auto info_ptr = ledger::Balance::New(info);
  double total = 0.0;
  for (auto & item : list) {
    total+=item->value;
  }
  info_ptr->total += total;
  info_ptr->wallets.insert(std::make_pair(ledger::kWalletUnBlinded, total));
  ExternalWallets(std::move(info_ptr), callback);
}

void WalletBalance::ExternalWallets(
    ledger::BalancePtr balance,
    ledger::FetchBalanceCallback callback) {
  if (!balance) {
    BLOG(0, "Balance is null");
    callback(ledger::Result::LEDGER_ERROR, std::move(balance));
    return;
  }

  auto wallets = ledger_->GetExternalWallets();

  if (wallets.empty()) {
    callback(ledger::Result::LEDGER_OK, std::move(balance));
    return;
  }

  auto uphold_callback = std::bind(&WalletBalance::OnUpholdFetchBalance,
      this,
      *balance,
      callback,
      _1,
      _2);

  uphold_->FetchBalance(uphold_callback);
}

void WalletBalance::OnUpholdFetchBalance(
    ledger::Balance info,
    ledger::FetchBalanceCallback callback,
    const ledger::Result result,
    const double balance) {
  ledger::BalancePtr info_ptr = ledger::Balance::New(info);

  if (result == ledger::Result::LEDGER_ERROR) {
    BLOG(0, "Can't get uphold balance");
    callback(ledger::Result::LEDGER_ERROR, std::move(info_ptr));
    return;
  }

  info_ptr->wallets.insert(std::make_pair(ledger::kWalletUphold, balance));
  info_ptr->total += balance;
  callback(result, std::move(info_ptr));
}

// static
double WalletBalance::GetPerWalletBalance(
    const std::string& type,
    base::flat_map<std::string, double> wallets) {
  if (type.empty() || wallets.size() == 0) {
    return 0.0;
  }

  for (const auto& wallet : wallets) {
    if (wallet.first == type) {
      return wallet.second;
    }
  }

  return  0.0;
}

}  // namespace braveledger_wallet
