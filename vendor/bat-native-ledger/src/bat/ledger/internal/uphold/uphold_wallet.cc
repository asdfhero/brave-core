/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <utility>

#include "base/json/json_reader.h"
#include "bat/ledger/global_constants.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/state/state_keys.h"
#include "bat/ledger/internal/uphold/uphold_util.h"
#include "bat/ledger/internal/uphold/uphold_wallet.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace braveledger_uphold {

UpholdWallet::UpholdWallet(bat_ledger::LedgerImpl* ledger, Uphold* uphold) :
    ledger_(ledger),
    uphold_(uphold) {
}

UpholdWallet::~UpholdWallet() = default;

void UpholdWallet::Generate(ledger::ExternalWalletCallback callback) {
  auto wallets = ledger_->GetExternalWallets();
  ledger::ExternalWalletPtr wallet;
  if (wallets.empty()) {
    wallet = ledger::ExternalWallet::New();
    wallet->status = ledger::WalletStatus::NOT_CONNECTED;
  } else {
    wallet = GetWallet(std::move(wallets));

    if (!wallet) {
      wallet = ledger::ExternalWallet::New();
      wallet->status = ledger::WalletStatus::NOT_CONNECTED;
    }
  }

  wallet->type = ledger::kWalletUphold;

  if (wallet->one_time_string.empty()) {
    wallet->one_time_string = GenerateRandomString(ledger::is_testing);
  }

  if (wallet->token.empty() &&
      (wallet->status == ledger::WalletStatus::PENDING ||
       wallet->status == ledger::WalletStatus::CONNECTED)) {
    wallet->status = ledger::WalletStatus::NOT_CONNECTED;
  }

  wallet = GenerateLinks(std::move(wallet));
  ledger_->SaveExternalWallet(ledger::kWalletUphold, wallet->Clone());

  if (wallet->status == ledger::WalletStatus::CONNECTED ||
      wallet->status == ledger::WalletStatus::VERIFIED ||
      wallet->status == ledger::WalletStatus::PENDING) {
    const auto user_callback = std::bind(&UpholdWallet::OnGenerate,
        this,
        _1,
        _2,
        callback);
    uphold_->GetUser(user_callback);
    return;
  }

  callback(ledger::Result::LEDGER_OK, std::move(wallet));
}

void UpholdWallet::OnGenerate(
    const ledger::Result result,
    const User& user,
    ledger::ExternalWalletCallback callback) {
  auto wallets = ledger_->GetExternalWallets();
  auto wallet_ptr = GetWallet(std::move(wallets));
  if (result == ledger::Result::EXPIRED_TOKEN) {
    uphold_->DisconnectWallet();
    callback(result, std::move(wallet_ptr));
    return;
  }

  if (user.bat_not_allowed) {
    BLOG(0, "BAT not allowed");
    callback(ledger::Result::BAT_NOT_ALLOWED, std::move(wallet_ptr));
    return;
  }

  if (!wallet_ptr || result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Wallet not generated");
    callback(result, std::move(wallet_ptr));
    return;
  }

  wallet_ptr->user_name = user.name;

  if (user.status != UserStatus::OK) {
    wallet_ptr->status = ledger::WalletStatus::PENDING;
  }

  wallet_ptr->status = GetNewStatus(wallet_ptr->status, user);
  ledger_->SaveExternalWallet(ledger::kWalletUphold, wallet_ptr->Clone());

  if (wallet_ptr->address.empty()) {
    auto card_callback = std::bind(&UpholdWallet::OnCreateCard,
        this,
        _1,
        _2,
        callback);
    uphold_->CreateCard(card_callback);
    return;
  }

  if (user.verified) {
    ledger_->TransferTokens([](const ledger::Result){});
    uphold_->TransferAnonToExternalWallet(callback);
    return;
  }

  callback(ledger::Result::LEDGER_OK, std::move(wallet_ptr));
}

void UpholdWallet::OnCreateCard(
    const ledger::Result result,
    const std::string& address,
    ledger::ExternalWalletCallback callback) {
  auto wallets = ledger_->GetExternalWallets();
  auto wallet_ptr = GetWallet(std::move(wallets));
  if (result != ledger::Result::LEDGER_OK || !wallet_ptr) {
    BLOG(0, "Card not created");
    callback(result, std::move(wallet_ptr));
    return;
  }

  wallet_ptr->address = address;
  wallet_ptr = GenerateLinks(std::move(wallet_ptr));
  ledger_->SaveExternalWallet(ledger::kWalletUphold, wallet_ptr->Clone());

  if (wallet_ptr->status == ledger::WalletStatus::VERIFIED) {
    ledger_->TransferTokens([](const ledger::Result){});
    uphold_->TransferAnonToExternalWallet(callback);
    return;
  }

  callback(ledger::Result::LEDGER_OK, std::move(wallet_ptr));
}

ledger::WalletStatus UpholdWallet::GetNewStatus(
    const ledger::WalletStatus old_status,
    const User& user) {
  ledger::WalletStatus new_status = old_status;
  switch (old_status) {
    case ledger::WalletStatus::CONNECTED: {
      if (!user.verified) {
        break;
      }
      new_status = ledger::WalletStatus::VERIFIED;
      ledger_->ShowNotification(
         "wallet_new_verified",
         [](ledger::Result _){},
         {"Uphold"});
      break;
    }
    case ledger::WalletStatus::VERIFIED: {
      if (user.verified) {
        break;
      }

      new_status = ledger::WalletStatus::CONNECTED;
      break;
    }
    case ledger::WalletStatus::PENDING: {
      if (user.status != UserStatus::OK) {
        break;
      }

      if (user.verified) {
        new_status = ledger::WalletStatus::VERIFIED;
        ledger_->ShowNotification(
           "wallet_new_verified",
           [](ledger::Result _){},
           {"Uphold"});
      } else {
        new_status = ledger::WalletStatus::CONNECTED;
      }
      break;
    }
    case ledger::WalletStatus::NOT_CONNECTED:
    case ledger::WalletStatus::DISCONNECTED_VERIFIED:
    case ledger::WalletStatus::DISCONNECTED_NOT_VERIFIED: {
      break;
    }
  }

  return new_status;
}

}  // namespace braveledger_uphold
