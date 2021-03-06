/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/content_settings/core/browser/brave_content_settings_ephemeral_provider.h"

#include <memory>
#include <utility>

#include "brave/components/content_settings/core/browser/brave_content_settings_utils.h"

namespace content_settings {

bool BraveEphemeralProvider::SetWebsiteSetting(
    const ContentSettingsPattern& primary_pattern,
    const ContentSettingsPattern& secondary_pattern,
    ContentSettingsType content_type,
    const ResourceIdentifier& resource_identifier,
    std::unique_ptr<base::Value>&& in_value,
    const ContentSettingConstraints& constraint) {
  // Prevent this handle shields configuration.
  if (content_type == ContentSettingsType::PLUGINS &&
      IsShieldsResourceID(resource_identifier)) {
    return false;
  }

  // Only flash plugin setting can be reached here.
  DCHECK(resource_identifier.empty());

  return EphemeralProvider::SetWebsiteSetting(
      primary_pattern, secondary_pattern, content_type, resource_identifier,
      std::move(in_value), constraint);
}

}  // namespace content_settings
