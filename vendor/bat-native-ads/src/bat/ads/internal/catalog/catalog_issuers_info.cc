/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/catalog/catalog_issuers_info.h"

namespace ads {

CatalogIssuersInfo::CatalogIssuersInfo() = default;

CatalogIssuersInfo::CatalogIssuersInfo(
    const CatalogIssuersInfo& info) = default;

CatalogIssuersInfo::~CatalogIssuersInfo() = default;

bool CatalogIssuersInfo::IsValid() const {
  if (public_key.empty() || issuers.empty()) {
    return false;
  }

  return true;
}

bool CatalogIssuersInfo::PublicKeyDoesExist(
    const std::string& public_key) const {
  if (this->public_key == public_key) {
    return true;
  }

  const auto iter = std::find_if(issuers.begin(), issuers.end(),
      [&public_key] (const auto& issuer) {
    return issuer.public_key == public_key;
  });

  if (iter == issuers.end()) {
    return false;
  }

  return true;
}

}  // namespace ads
