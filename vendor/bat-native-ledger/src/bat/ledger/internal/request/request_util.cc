/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <map>

#include "base/strings/stringprintf.h"
#include "bat/ledger/ledger.h"
#include "bat/ledger/internal/static_values.h"
#include "bat/ledger/internal/request/request_util.h"
#include "bat/ledger/internal/common/security_helper.h"

namespace braveledger_request_util {

namespace ledger {
const char kDevelopment[] = "https://ledger.rewards.brave.software";
const char kStaging[] =
    "https://ledger-staging.mercury.basicattentiontoken.org";
const char kProduction[] = "https://ledger.mercury.basicattentiontoken.org";
}  // namespace ledger

namespace balance {

const char kDevelopment[] = "https://balance.rewards.brave.software";
const char kStaging[] =
    "https:kP/balance-staging.mercury.basicattentiontoken.org";
const char kProduction[] = "https://balance.mercury.basicattentiontoken.org";
}  // namespace balance

namespace publisher {
const char kDevelopment[] = "https://rewards-dev.brave.software";
const char kStaging[] = "https://rewards-stg.bravesoftware.com";
const char kProduction[] = "https://rewards.brave.com";
}  // namespace publisher

namespace promotion {
const char kDevelopment[] = "https://grant.rewards.brave.software";
const char kStaging[] = "https://grant.rewards.bravesoftware.com";
const char kProduction[] = "https://grant.rewards.brave.com";
}  // namespace promotion

namespace payment {
const char kDevelopment[] = "https://payment.rewards.brave.software";
const char kStaging[] = "http://payment.rewards.bravesoftware.com";
const char kProduction[] = "http://payment.rewards.brave.com";
}  // namespace payment

namespace api {
const char kDevelopment[] = "https://api.rewards.brave.software";
const char kStaging[] = "https://api.rewards.bravesoftware.com";
const char kProduction[] = "https://api.rewards.brave.com";
}  // namespace api

namespace cdn {
const char kDevelopment[] = "https://pcdn.brave.software";
const char kStaging[] = "https://pcdn.bravesoftware.com";
const char kProduction[] = "https://pcdn.brave.com";
}  // namespace cdn

std::string BuildBalanceUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = balance::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = balance::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = balance::kProduction;
      break;
  }

  return url;
}

std::string BuildPublisherUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = publisher::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = publisher::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = publisher::kProduction;
      break;
  }

  return url;
}

std::string BuildLedgerUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = ledger::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = ledger::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = ledger::kProduction;
      break;
  }

  return url;
}

std::string BuildPromotionUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = promotion::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = promotion::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = promotion::kProduction;
      break;
  }

  return url;
}

std::string BuildPaymentsUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = payment::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = payment::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = payment::kProduction;
      break;
  }

  return url;
}

std::string BuildApiUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = api::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = api::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = api::kProduction;
      break;
  }

  return url;
}

std::string BuildPrivateCdnUrl() {
  std::string url;
  switch (::ledger::_environment) {
    case ::ledger::Environment::DEVELOPMENT:
      url = cdn::kDevelopment;
      break;
    case ::ledger::Environment::STAGING:
      url = cdn::kStaging;
      break;
    case ::ledger::Environment::PRODUCTION:
      url = cdn::kProduction;
      break;
  }

  return url;
}

std::string BuildUrl(
    const std::string& path,
    const std::string& prefix,
    const ServerTypes& server) {
  std::string url;
  switch (server) {
    case ServerTypes::BALANCE: {
      url = BuildBalanceUrl();
      break;
    }
    case ServerTypes::kPublisher: {
      url = BuildPublisherUrl();
      break;
    }
    case ServerTypes::LEDGER: {
      url = BuildLedgerUrl();
      break;
    }
    case ServerTypes::kPromotion: {
      url = BuildPromotionUrl();
      break;
    }
    case ServerTypes::kPayments: {
      url = BuildPaymentsUrl();
      break;
    }
    case ServerTypes::kAPI: {
      url = BuildApiUrl();
      break;
    }
    case ServerTypes::kPrivateCDN: {
      url = BuildPrivateCdnUrl();
      break;
    }
  }

  if (url.empty()) {
    NOTREACHED();
    return "";
  }

  return url + prefix + path;
}

std::string DigestValue(const std::string& body) {
  const auto body_sha256 = braveledger_helper::Security::GetSHA256(body);
  const auto body_sha256_base64 =
      braveledger_helper::Security::GetBase64(body_sha256);

  return base::StringPrintf("SHA-256=%s", body_sha256_base64.c_str());
}

std::string SignatureHeaderValue(
    const std::string& data,
    const std::string& body,
    const std::string key_id,
    const std::vector<uint8_t>& private_key,
    const bool idempotency_key) {
  DCHECK(!body.empty());
  DCHECK(!private_key.empty());

  auto digest_header_value = DigestValue(body);

  std::vector<std::map<std::string, std::string>> headers;
  headers.push_back({{"digest", digest_header_value}});
  if (idempotency_key) {
    headers.push_back({{"idempotency-key", data}});
  } else {
    headers.push_back({{"(request-target)", data}});
  }

  return braveledger_helper::Security::Sign(
      headers,
      key_id,
      private_key);
}

std::map<std::string, std::string> GetSignHeaders(
    const std::string& data,
    const std::string& body,
    const std::string& key_id,
    const std::vector<uint8_t>& private_key,
    const bool idempotency_key) {
  const std::string digest_header = DigestValue(body).c_str();
  const std::string signature_header = SignatureHeaderValue(
      data,
      body,
      key_id,
      private_key,
      idempotency_key).c_str();

  return {
    { "digest", digest_header },
    { "signature", signature_header}
  };
}

std::vector<std::string> BuildSignHeaders(
    const std::string& url,
    const std::string& body,
    const std::string& key_id,
    const std::vector<uint8_t>& private_key) {
  auto headers = GetSignHeaders(url, body, key_id, private_key);
  DCHECK_EQ(headers.size(), 2ul);

  const std::string digest_header = base::StringPrintf(
      "digest: %s",
      headers.at("digest").c_str());
  const std::string signature_header = base::StringPrintf(
      "signature: %s",
      headers.at("signature").c_str());

  const std::string accept_header = "accept: application/json";

  return {
    digest_header,
    signature_header,
    accept_header
  };
}

}  // namespace braveledger_request_util
