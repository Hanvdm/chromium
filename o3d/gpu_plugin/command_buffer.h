// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef O3D_GPU_PLUGIN_COMMAND_BUFFER_H_
#define O3D_GPU_PLUGIN_COMMAND_BUFFER_H_

#include <set>
#include <vector>

#include "base/message_loop.h"
#include "base/scoped_ptr.h"
#include "base/task.h"
#include "o3d/gpu_plugin/np_utils/default_np_object.h"
#include "o3d/gpu_plugin/np_utils/np_dispatcher.h"
#include "o3d/gpu_plugin/system_services/shared_memory_public.h"

namespace o3d {
namespace gpu_plugin {

// An NPObject that implements a shared memory command buffer and a synchronous
// API to manage the put and get pointers.
class CommandBuffer : public DefaultNPObject<NPObject> {
 public:
  enum {
    ERROR_NO_ERROR,
    ERROR_INVALID_SIZE,
    ERROR_OUT_OF_BOUNDS,
    ERROR_UNKNOWN_COMMAND,
    ERROR_INVALID_ARGUMENTS,
  };

  explicit CommandBuffer(NPP npp);
  virtual ~CommandBuffer();

  // Create a shared memory buffer of the given size.
  virtual bool Initialize(int32 size);

  // Gets the shared memory ring buffer object for the command buffer.
  virtual NPObjectPointer<CHRSharedMemory> GetRingBuffer();

  virtual int32 GetSize();

  // The writer calls this to update its put offset. This function returns the
  // reader's most recent get offset. Does not return until after the put offset
  // change callback has been invoked. Returns -1 if the put offset is invalid.
  virtual int32 SyncOffsets(int32 put_offset);

  // Returns the current get offset. This can be called from any thread.
  virtual int32 GetGetOffset();

  // Sets the current get offset. This can be called from any thread.
  virtual void SetGetOffset(int32 get_offset);

  // Returns the current put offset. This can be called from any thread.
  virtual int32 GetPutOffset();

  // Sets a callback that should be posted on another thread whenever the put
  // offset is changed. The callback must not return until some progress has
  // been made (unless the command buffer is empty), i.e. the
  // get offset must have changed. It need not process the entire command
  // buffer though. This allows concurrency between the writer and the reader
  // while giving the writer a means of waiting for the reader to make some
  // progress before attempting to write more to the command buffer. Avoiding
  // the use of a synchronization primitive like a condition variable to
  // synchronize reader and writer reduces the risk of deadlock.
  // Takes ownership of callback. The callback is invoked on the plugin thread.
  virtual void SetPutOffsetChangeCallback(Callback0::Type* callback);

  // Get an opaque integer handle for an NPObject. This can be used
  // to identify the shared memory object from the ring buffer. Note that the
  // object will be retained. Consider reference cycle issues. Returns zero for
  // NULL, positive for non-NULL and -1 on error. Objects may be registered
  // multiple times and have multiple associated handles. Each handle for a
  // distinct object must be separately unregistered.
  virtual int32 RegisterObject(NPObjectPointer<NPObject> object);

  // Unregister a previously registered NPObject. It is safe to unregister the
  // zero handle.
  virtual void UnregisterObject(NPObjectPointer<NPObject> object, int32 handle);

  virtual NPObjectPointer<NPObject> GetRegisteredObject(int32 handle);

  // Get the current token value. This is used for by the writer to defer
  // changes to shared memory objects until the reader has reached a certain
  // point in the command buffer. The reader is responsible for updating the
  // token value, for example in response to an asynchronous set token command
  // embedded in the command buffer. The default token value is zero.
  int32 GetToken() {
    return token_;
  }

  // Allows the reader to update the current token value.
  void SetToken(int32 token) {
    token_ = token;
  }

  // Get the current error status and reset it to ERROR_NO_ERROR.
  // The default error status is ERROR_NO_ERROR.
  int32 ResetError();

  // Allows the reader to set the current error status.
  void SetError(int32 error) {
    error_ = error;
  }

  NP_UTILS_BEGIN_DISPATCHER_CHAIN(CommandBuffer, DefaultNPObject<NPObject>)
    NP_UTILS_DISPATCHER(Initialize, bool(int32))
    NP_UTILS_DISPATCHER(GetRingBuffer, NPObjectPointer<CHRSharedMemory>())
    NP_UTILS_DISPATCHER(GetSize, int32())
    NP_UTILS_DISPATCHER(SyncOffsets, int32(int32))
    NP_UTILS_DISPATCHER(GetGetOffset, int32());
    NP_UTILS_DISPATCHER(GetPutOffset, int32());
    NP_UTILS_DISPATCHER(RegisterObject, int32(NPObjectPointer<NPObject>));
    NP_UTILS_DISPATCHER(UnregisterObject,
                        void(NPObjectPointer<NPObject>, int32));
    NP_UTILS_DISPATCHER(GetToken, int32());
    NP_UTILS_DISPATCHER(ResetError, int32());
  NP_UTILS_END_DISPATCHER_CHAIN

 private:
  NPP npp_;
  NPObjectPointer<CHRSharedMemory> shared_memory_;
  int32 size_;
  int32 get_offset_;
  int32 put_offset_;
  scoped_ptr<Callback0::Type> put_offset_change_callback_;
  std::vector<NPObjectPointer<NPObject> > registered_objects_;
  std::set<int32> unused_registered_object_elements_;
  int32 token_;
  int32 error_;
};

}  // namespace gpu_plugin
}  // namespace o3d

#endif  // O3D_GPU_PLUGIN_COMMAND_BUFFER_H_
