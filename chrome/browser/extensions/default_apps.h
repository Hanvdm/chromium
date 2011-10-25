// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_
#define CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_
#pragma once

#include "base/basictypes.h"

class PrefService;
class Profile;

// Functions and types related to installing default apps.
namespace default_apps {

// These enum values are persisted in the user preferences, so they should not
// be changed.
enum InstallState {
  kUnknown,
  kAlwaysProvideDefaultApps,
  kNeverProvideDefaultApps
};

// Register preference properties used by default apps to maintain
// install state.
void RegisterUserPrefs(PrefService* prefs);

// Determines whether default apps should be installed into the specified
// profile.  If true, then an instance of ExternalExtensionProviderImpl
// specific to default apps should be added to the external providers list.
bool ShouldInstallInProfile(Profile* profile);

}  // namespace default_apps

#endif  // CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_
