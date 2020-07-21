/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/search_engines/brave_prepopulated_engines.h"

#include "components/search_engines/prepopulated_engines.h"
#include "components/search_engines/search_engine_type.h"

namespace TemplateURLPrepopulateData {

// IMPORTANT! Make sure to bump this value if you make changes to the
// engines below or add/remove engines.
const int kBraveCurrentDataVersion = 5;

PrepopulatedEngine ModifyEngineParams(const PrepopulatedEngine& engine,
    const wchar_t* const name,
    const wchar_t* const keyword, const char* const search_url,
    const char* const suggest_url, int id) {
  return {
    name ? name : engine.name,
    keyword ? keyword : engine.keyword,
    engine.favicon_url,
    search_url ? search_url : engine.search_url,
    engine.encoding,
    suggest_url ? suggest_url : engine.suggest_url,
    engine.image_url,
    engine.new_tab_url,
    engine.contextual_search_url,
    engine.logo_url,
    engine.doodle_url,
    engine.search_url_post_params,
    engine.suggest_url_post_params,
    engine.image_url_post_params,
    engine.alternate_urls,
    engine.alternate_urls_size,
    engine.type,
    id > 0 ? id : engine.id
  };
}

const PrepopulatedEngine duckduckgo = {
  L"DuckDuckGo",
  L":d",
  "https://duckduckgo.com/favicon.ico",
  "https://duckduckgo.com/?q={searchTerms}&t=brave",
  "UTF-8",
  "https://ac.duckduckgo.com/ac/?q={searchTerms}&type=list",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  SEARCH_ENGINE_DUCKDUCKGO,
  PREPOPULATED_ENGINE_ID_DUCKDUCKGO,
};

const PrepopulatedEngine duckduckgo_de = ModifyEngineParams(
  duckduckgo,
  NULL,
  NULL,
  "https://duckduckgo.com/?q={searchTerms}&t=bravened",
  NULL,
  PREPOPULATED_ENGINE_ID_DUCKDUCKGO_DE);

const PrepopulatedEngine duckduckgo_au_nz_ie = ModifyEngineParams(
  duckduckgo,
  NULL,
  NULL,
  "https://duckduckgo.com/?q={searchTerms}&t=braveed",
  NULL,
  PREPOPULATED_ENGINE_ID_DUCKDUCKGO_AU_NZ_IE);

#if defined(OS_ANDROID)
const PrepopulatedEngine duckduckgo_lite = {
  L"DuckDuckGo Lite",
  L":dl",
  "https://duckduckgo.com/favicon.ico",
  "https://duckduckgo.com/lite/?q={searchTerms}&t=brave",
  "UTF-8",
  "https://ac.duckduckgo.com/ac/?q={searchTerms}&type=list",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  SEARCH_ENGINE_DUCKDUCKGO,
  PREPOPULATED_ENGINE_ID_DUCKDUCKGO_LITE,
};
#endif

const PrepopulatedEngine qwant = {
  L"Qwant",
  L":q",
  "https://www.qwant.com/favicon.ico",
  "https://www.qwant.com/?q={searchTerms}&client=brz-brave",
  "UTF-8",
  "https://api.qwant.com/api/suggest/?q={searchTerms}&client=opensearch",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  SEARCH_ENGINE_QWANT,
  PREPOPULATED_ENGINE_ID_QWANT,
};

const PrepopulatedEngine startpage = {
    L"StartPage",
    L":sp",
    "https://www.startpage.com/favicon.ico",
    "https://www.startpage.com/do/"
    "dsearch?query={searchTerms}&cat=web&pl=opensearch",
    "UTF-8",
    "https://www.startpage.com/cgi-bin/"
    "csuggest?query={searchTerms}&limit=10&format=json",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    SEARCH_ENGINE_OTHER,
    PREPOPULATED_ENGINE_ID_STARTPAGE,
};

const wchar_t BRAVE_YAHOO_NAME[] = L"Yahoo";
const wchar_t BRAVE_YAHOO_KEYWORD[] = L":y";

const PrepopulatedEngine brave_yahoo = ModifyEngineParams(
  yahoo,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  "https://search.yahoo.com/search?p={searchTerms}&fr=brave_yset",
  "https://search.yahoo.com/sugg/chrome"
  "?output=fxjson&command={searchTerms}&fr=brave_yset",
  PREPOPULATED_ENGINE_ID_YAHOO);

const PrepopulatedEngine brave_yahoo_ar = ModifyEngineParams(
  yahoo_ar,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_AR);

const PrepopulatedEngine brave_yahoo_at = ModifyEngineParams(
  yahoo_at,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_AT);

const PrepopulatedEngine brave_yahoo_au = ModifyEngineParams(
  yahoo_au,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_AU);

const PrepopulatedEngine brave_yahoo_br = ModifyEngineParams(
  yahoo_br,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_BR);

const PrepopulatedEngine brave_yahoo_ca = ModifyEngineParams(
  yahoo_ca,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_CA);

const PrepopulatedEngine brave_yahoo_ch = ModifyEngineParams(
  yahoo_ch,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_CH);

const PrepopulatedEngine brave_yahoo_cl = ModifyEngineParams(
  yahoo_cl,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_CL);

const PrepopulatedEngine brave_yahoo_co = ModifyEngineParams(
  yahoo_co,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_CO);

const PrepopulatedEngine brave_yahoo_de = ModifyEngineParams(
  yahoo_de,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_DE);

const PrepopulatedEngine brave_yahoo_dk = ModifyEngineParams(
  yahoo_dk,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_DK);

const PrepopulatedEngine brave_yahoo_es = ModifyEngineParams(
  yahoo_es,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_ES);

const PrepopulatedEngine brave_yahoo_fi = ModifyEngineParams(
  yahoo_fi,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_FI);

const PrepopulatedEngine brave_yahoo_fr = ModifyEngineParams(
  yahoo_fr,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_FR);

const PrepopulatedEngine brave_yahoo_hk = ModifyEngineParams(
  yahoo_hk,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_HK);

const PrepopulatedEngine brave_yahoo_id = ModifyEngineParams(
  yahoo_id,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_ID);

const PrepopulatedEngine brave_yahoo_in = ModifyEngineParams(
  yahoo_in,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_IN);

const PrepopulatedEngine brave_yahoo_jp = ModifyEngineParams(
  yahoo_jp,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_JP);

const PrepopulatedEngine brave_yahoo_mx = ModifyEngineParams(
  yahoo_mx,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_MX);

const PrepopulatedEngine brave_yahoo_my = ModifyEngineParams(
  yahoo_my,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_MY);

const PrepopulatedEngine brave_yahoo_nl = ModifyEngineParams(
  yahoo_nl,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_NL);

const PrepopulatedEngine brave_yahoo_nz = ModifyEngineParams(
  yahoo_nz,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_NZ);

const PrepopulatedEngine brave_yahoo_pe = ModifyEngineParams(
  yahoo_pe,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_PE);

const PrepopulatedEngine brave_yahoo_ph = ModifyEngineParams(
  yahoo_ph,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_PH);

const PrepopulatedEngine brave_yahoo_qc = ModifyEngineParams(
  yahoo_qc,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_QC);

const PrepopulatedEngine brave_yahoo_se = ModifyEngineParams(
  yahoo_se,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_SE);

const PrepopulatedEngine brave_yahoo_sg = ModifyEngineParams(
  yahoo_sg,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_SG);

const PrepopulatedEngine brave_yahoo_th = ModifyEngineParams(
  yahoo_th,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_TH);

const PrepopulatedEngine brave_yahoo_tr = ModifyEngineParams(
  yahoo_tr,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_TR);

const PrepopulatedEngine brave_yahoo_tw = ModifyEngineParams(
  yahoo_tw,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_TW);

const PrepopulatedEngine brave_yahoo_uk = ModifyEngineParams(
  yahoo_uk,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_UK);

const PrepopulatedEngine brave_yahoo_ve = ModifyEngineParams(
  yahoo_ve,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_VE);

const PrepopulatedEngine brave_yahoo_vn = ModifyEngineParams(
  yahoo_vn,
  BRAVE_YAHOO_NAME,
  BRAVE_YAHOO_KEYWORD,
  0,
  0,
  PREPOPULATED_ENGINE_ID_YAHOO_VN);

}  // namespace TemplateURLPrepopulateData
