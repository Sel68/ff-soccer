#include "MessageInterface.h"

#include <cstring>
#include <string>

#include "Logger.h"
#include "message.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

bool PayloadDecodeCallback(pb_istream_t* stream, const pb_field_t* field, void** arg) {
  StringPayloadDecode* sp = (StringPayloadDecode*)(*arg);
  size_t bytes_to_read = stream->bytes_left;

  if (bytes_to_read > sp->buffer_size) {
    debug::Log() << "[PayloadDecodeCallback]: Not enough space in buffer\r\n";
    return false;
  }

  if (!pb_read(stream, sp->buffer + *(sp->bytes_written), bytes_to_read)) {
    debug::Log() << "[PayloadDecodeCallback]: pb_read failed\r\n";
    return false;
  }

  *(sp->bytes_written) += bytes_to_read;
  return true;
}

bool PayloadEncodeCallback(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
  StringPayload* payload = (StringPayload*)(*arg);

  // VERY IMPORTANT. TOOK ME A DAY
  if (!pb_encode_tag_for_field(stream, field)) {
    debug::Log() << "[PayloadEncodeCallback]: Failed to encode length\r\n";
    return false;
  }

  bool status = pb_encode_string(stream, payload->data, payload->size);
  if (!status) {
    debug::Log() << "[PayloadEncodeCallback]: pb_write failed\r\n";
    return false;
  }
  return status;
}