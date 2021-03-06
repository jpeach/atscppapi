/*
 * Copyright (c) 2013 LinkedIn Corp. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the license at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 */

/**
 * @file RemapPlugin.cc
 * @author Brian Geffon
 * @author Manjesh Nilange
 */

#include "atscppapi/RemapPlugin.h"
#include "logging_internal.h"
#include "utils_internal.h"
#include <assert.h>
#include <ts/remap.h>

using namespace atscppapi;

TSRemapStatus TSRemapDoRemap(void* ih, TSHttpTxn rh, TSRemapRequestInfo* rri) {
  RemapPlugin *remap_plugin = static_cast<RemapPlugin *>(ih);
  Url map_from_url(rri->requestBufp, rri->mapFromUrl), map_to_url(rri->requestBufp, rri->mapToUrl);
  Transaction &transaction = utils::internal::getTransaction(rh);
  bool redirect = false;
  RemapPlugin::Result result = remap_plugin->doRemap(map_from_url, map_to_url, transaction, redirect);
  rri->redirect = redirect ? 1 : 0;
  switch (result) {
  case RemapPlugin::RESULT_ERROR:
    return TSREMAP_ERROR;
  case RemapPlugin::RESULT_NO_REMAP:
    return TSREMAP_NO_REMAP;
  case RemapPlugin::RESULT_DID_REMAP:
    return TSREMAP_DID_REMAP;
  case RemapPlugin::RESULT_NO_REMAP_STOP:
    return TSREMAP_NO_REMAP_STOP;
  case RemapPlugin::RESULT_DID_REMAP_STOP:
    return TSREMAP_DID_REMAP_STOP;
  default:
    assert(!"Unhandled result");
    return TSREMAP_ERROR;
  }
}

void TSRemapDeleteInstance(void *ih) {
  RemapPlugin *remap_plugin = static_cast<RemapPlugin *>(ih);
  delete remap_plugin;
}

TSReturnCode TSRemapInit(TSRemapInterface *api_info, char *errbuf, int errbuf_size) {
  return TS_SUCCESS;
}

RemapPlugin::RemapPlugin(void **instance_handle) {
  *instance_handle = static_cast<void *>(this);
}

