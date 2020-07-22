/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_TOR_TOR_TAB_HELPER_H_
#define BRAVE_BROWSER_TOR_TOR_TAB_HELPER_H_

#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class NavigationHandle;
class WebContents;
}  // namespace content

namespace tor {

class TorTabHelper : public content::WebContentsObserver,
                     public content::WebContentsUserData<TorTabHelper> {
 public:
  ~TorTabHelper() override;

  static void MaybeCreateForWebContents(content::WebContents* web_contents);

 private:
  friend class content::WebContentsUserData<TorTabHelper>;
  explicit TorTabHelper(content::WebContents* web_contents);

  // content::WebContentsObserver
  void DidFinishNavigation(
      content::NavigationHandle* navigation_handle) override;

  int retry_count_;

  WEB_CONTENTS_USER_DATA_KEY_DECL();

  DISALLOW_COPY_AND_ASSIGN(TorTabHelper);
};

}  // namespace tor

#endif  // BRAVE_BROWSER_TOR_TOR_TAB_HELPER_H_
