/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_CATALOG_ISSUERS_INFO_H_
#define BAT_ADS_CATALOG_ISSUERS_INFO_H_

#include <string>

#include "bat/ads/internal/catalog/catalog_issuer_info.h"

namespace ads {

struct CatalogIssuersInfo {
  CatalogIssuersInfo();
  CatalogIssuersInfo(
      const CatalogIssuersInfo& info);
  ~CatalogIssuersInfo();

  bool IsValid() const;

  bool PublicKeyDoesExist(
      const std::string& public_key) const;

  std::string public_key;
  CatalogIssuerList issuers;
};

}  // namespace ads

#endif  // BAT_ADS_CATALOG_ISSUERS_INFO_H_
