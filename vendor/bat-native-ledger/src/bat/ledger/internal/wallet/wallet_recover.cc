/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/wallet/wallet_recover.h"

#include <utility>

#include "base/json/json_reader.h"
#include "base/strings/string_split.h"
#include "bat/ledger/internal/common/security_helper.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/request/request_promotion.h"
#include "bat/ledger/internal/request/request_util.h"
#include "bat/ledger/internal/state/state_keys.h"
#include "bat/ledger/internal/state/state_util.h"
#include "net/http/http_status_code.h"

#include "wally_bip39.h"  // NOLINT

using std::placeholders::_1;

namespace {

ledger::Result ParseResponse(
    const std::string& response,
    std::string* payment_id) {
  DCHECK(payment_id);

  base::Optional<base::Value> value = base::JSONReader::Read(response);
  if (!value || !value->is_dict()) {
    return ledger::Result::LEDGER_ERROR;
  }

  base::DictionaryValue* dictionary = nullptr;
  if (!value->GetAsDictionary(&dictionary)) {
    return ledger::Result::LEDGER_ERROR;
  }

  const auto* payment_id_string = dictionary->FindStringKey("paymentId");
  if (!payment_id_string || payment_id_string->empty()) {
    BLOG(0, "Payment id is missing");
    return ledger::Result::LEDGER_ERROR;
  }

  *payment_id = *payment_id_string;
  return ledger::Result::LEDGER_OK;
}

}  // namespace

namespace braveledger_wallet {

WalletRecover::WalletRecover(bat_ledger::LedgerImpl* ledger) : ledger_(ledger) {
}

WalletRecover::~WalletRecover() = default;

void WalletRecover::Start(
    const std::string& pass_phrase,
    ledger::ResultCallback callback) {
  if (pass_phrase.empty()) {
    BLOG(0, "Pass phrase is empty");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  auto phrase_split = base::SplitString(
      pass_phrase,
      " ",
      base::TRIM_WHITESPACE,
      base::SPLIT_WANT_NONEMPTY);

  if (phrase_split.size() == 16) {
    BLOG(0, "Pass phrase is niceware, update to bip39");
    callback(ledger::Result::CORRUPTED_DATA);
    return;
  }

  std::vector<unsigned char> new_seed;
  new_seed.resize(32);
  size_t written = 0;
  const int result = bip39_mnemonic_to_bytes(
      nullptr,
      pass_phrase.c_str(),
      &new_seed.front(),
      new_seed.size(),
      &written);

  if (result != 0 || written == 0) {
    BLOG(0, "Result: " << result << " Size: " << written);
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  const std::string public_key_hex =
      braveledger_helper::Security::GetPublicKeyHexFromSeed(new_seed);

  auto url_callback = std::bind(&WalletRecover::OnRecover,
      this,
      _1,
      new_seed,
      callback);

  const std::string url =
      braveledger_request_util::GetRecoverWalletURL(public_key_hex);
  ledger_->LoadURL(
      url,
      {},
      "",
      "application/json; charset=utf-8",
      ledger::UrlMethod::GET,
      url_callback);
}

void WalletRecover::OnRecover(
    const ledger::UrlResponse& response,
    const std::vector<uint8_t>& new_seed,
    ledger::ResultCallback callback) {
  BLOG(6, ledger::UrlResponseToString(__func__, response));

  if (response.status_code != net::HTTP_OK) {
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  std::string payment_id;
  const auto result = ParseResponse(response.body, &payment_id);
  if (result == ledger::Result::LEDGER_ERROR) {
    BLOG(0, "Couldn't parse response");
    callback(result);
    return;
  }

  braveledger_state::SetRecoverySeed(ledger_, new_seed);
  braveledger_state::SetPaymentId(ledger_, payment_id);
  braveledger_state::SetFetchOldBalanceEnabled(ledger_, true);
  ledger_->SetBooleanState(ledger::kStateAnonTransferChecked, false);
  ledger_->SetUint64State(ledger::kStatePromotionLastFetchStamp, 0);
  ledger_->SetConfirmationsWalletInfo();

  callback(ledger::Result::LEDGER_OK);
}

}  // namespace braveledger_wallet
