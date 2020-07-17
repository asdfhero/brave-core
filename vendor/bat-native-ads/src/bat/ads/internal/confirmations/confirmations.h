/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_
#define BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_

#include <memory>
#include <string>

#include "bat/ads/ads.h"
#include "bat/ads/internal/catalog/catalog_issuers_info.h"
#include "bat/ads/internal/confirmations/confirmations_state.h"
#include "bat/ads/internal/privacy/unblinded_tokens/unblinded_token_info.h"
#include "bat/ads/internal/privacy/unblinded_tokens/unblinded_tokens.h"
#include "bat/ads/result.h"

namespace ads {

class AdsImpl;

class Confirmations {
 public:
  Confirmations(
      AdsImpl* ads);

  ~Confirmations();

  void Initialize(
      InitializeCallback callback);

  bool is_initialized_ = false;

  InitializeCallback callback_;

  CatalogIssuersInfo GetCatalogIssuers() const;
  void SetCatalogIssuers(
      const CatalogIssuersInfo& catalog_issuers);

  ConfirmationList GetConfirmations() const;
  void SetConfirmations(
      const ConfirmationList& confirmations);
  void AppendConfirmationToRetryQueue(
      const ConfirmationInfo& confirmation);
  void RemoveConfirmationFromRetryQueue(
      const ConfirmationInfo& confirmation);

  TransactionList GetTransactions() const;
  void SetTransactions(
      const TransactionList& transactions);
  void AppendTransaction(
      const double estimated_redemption_value,
      const ConfirmationType confirmation_type);

  uint64_t GetNextTokenRedemptionTimestamp() const;
  void SetNextTokenRedemptionTimestamp(
      const uint64_t next_token_redemption_timestamp);

//   base::Value get_ad_rewards() const;
//   void set_ad_rewards(
//       const base::Value& value);

  privacy::UnblindedTokens* get_unblinded_tokens();

  privacy::UnblindedTokens* get_unblinded_payment_tokens();

  void SaveState();

  void LoadState();

 private:
  void OnStateSaved(
      const Result result);

  void OnStateLoaded(
      const Result result,
      const std::string& json);

  AdsImpl* ads_;  // NOT OWNED

  std::unique_ptr<ConfirmationsState> state_;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_
