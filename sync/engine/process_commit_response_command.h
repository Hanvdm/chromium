// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SYNC_ENGINE_PROCESS_COMMIT_RESPONSE_COMMAND_H_
#define SYNC_ENGINE_PROCESS_COMMIT_RESPONSE_COMMAND_H_
#pragma once

#include <set>
#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "sync/engine/model_changing_syncer_command.h"
#include "sync/engine/syncproto.h"

namespace syncable {
class Id;
class WriteTransaction;
class MutableEntry;
}

namespace browser_sync {

class ProcessCommitResponseCommand : public ModelChangingSyncerCommand {
 public:

  ProcessCommitResponseCommand();
  virtual ~ProcessCommitResponseCommand();

 protected:
  // ModelChangingSyncerCommand implementation.
  virtual std::set<ModelSafeGroup> GetGroupsToChange(
      const sessions::SyncSession& session) const OVERRIDE;
  virtual SyncerError ModelNeutralExecuteImpl(
      sessions::SyncSession* session) OVERRIDE;
  virtual SyncerError ModelChangingExecuteImpl(
      sessions::SyncSession* session) OVERRIDE;

 private:
  CommitResponse::ResponseType ProcessSingleCommitResponse(
      syncable::WriteTransaction* trans,
      const sync_pb::CommitResponse_EntryResponse& pb_commit_response,
      const sync_pb::SyncEntity& pb_committed_entry,
      const syncable::Id& pre_commit_id,
      std::set<syncable::Id>* conflicting_new_directory_ids,
      std::set<syncable::Id>* deleted_folders);

  // Actually does the work of execute.
  SyncerError ProcessCommitResponse(sessions::SyncSession* session);

  void ProcessSuccessfulCommitResponse(
      const sync_pb::SyncEntity& committed_entry,
      const CommitResponse_EntryResponse& entry_response,
      const syncable::Id& pre_commit_id, syncable::MutableEntry* local_entry,
      bool syncing_was_set, std::set<syncable::Id>* deleted_folders);

  // Update the BASE_VERSION and SERVER_VERSION, post-commit.
  // Helper for ProcessSuccessfulCommitResponse.
  bool UpdateVersionAfterCommit(
      const sync_pb::SyncEntity& committed_entry,
      const CommitResponse_EntryResponse& entry_response,
      const syncable::Id& pre_commit_id,
      syncable::MutableEntry* local_entry);

  // If the server generated an ID for us during a commit, apply the new ID.
  // Helper for ProcessSuccessfulCommitResponse.
  bool ChangeIdAfterCommit(
      const CommitResponse_EntryResponse& entry_response,
      const syncable::Id& pre_commit_id,
      syncable::MutableEntry* local_entry);

  // Update the SERVER_ fields to reflect the server state after committing.
  // Helper for ProcessSuccessfulCommitResponse.
  void UpdateServerFieldsAfterCommit(
      const sync_pb::SyncEntity& committed_entry,
      const CommitResponse_EntryResponse& entry_response,
      syncable::MutableEntry* local_entry);

  // The server can override some values during a commit; the overridden values
  // are returned as fields in the CommitResponse_EntryResponse.  This method
  // stores the fields back in the client-visible (i.e. not the SERVER_* fields)
  // fields of the entry.  This should only be done if the item did not change
  // locally while the commit was in flight.
  // Helper for ProcessSuccessfulCommitResponse.
  void OverrideClientFieldsAfterCommit(
      const sync_pb::SyncEntity& committed_entry,
      const CommitResponse_EntryResponse& entry_response,
      syncable::MutableEntry* local_entry);

  // Helper to extract the final name from the protobufs.
  const std::string& GetResultingPostCommitName(
      const sync_pb::SyncEntity& committed_entry,
      const CommitResponse_EntryResponse& entry_response);

  DISALLOW_COPY_AND_ASSIGN(ProcessCommitResponseCommand);
};

}  // namespace browser_sync

#endif  // SYNC_ENGINE_PROCESS_COMMIT_RESPONSE_COMMAND_H_
