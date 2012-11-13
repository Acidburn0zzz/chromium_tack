// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_HASH_VALUE_H_
#define NET_BASE_HASH_VALUE_H_

#include <string.h>

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "build/build_config.h"

namespace net {

struct SHA1HashValue {
  bool Equals(const SHA1HashValue& other) const {
    return memcmp(data, other.data, sizeof(data)) == 0;
  }

  unsigned char data[20];
};

struct SHA256HashValue {
  bool Equals(const SHA256HashValue& other) const {
    return memcmp(data, other.data, sizeof(data)) == 0;
  }

  unsigned char data[32];
};

enum HashValueTag {
  HASH_VALUE_SHA1,
  HASH_VALUE_SHA256,

  // This must always be last.
  HASH_VALUE_TAGS_COUNT
};

class HashValue {
 public:
  explicit HashValue(HashValueTag tag) : tag(tag) {}
  HashValue() : tag(HASH_VALUE_SHA1) {}

  bool Equals(const HashValue& other) const;

  // Parse/write in this format: "sha1/Guzek9lMwR3KeIS8wwS9gBvVtIg="
  // i.e. <hash-name>"/"<base64-hash-value>
  // This format is used for:
  //   - net_internals display/setting public-key pins
  //   - logging public-key pins
  //   - serializing public-key pins
  //
  // FromString() parse errors SHALL return false, and MAY leave the
  //   HashValue containing incorrect data
  // ToString() errors (ie unknown tag) returns "unknown/"<base64>
  //   (but ToString() errors should not occur!)
  bool FromString(const std::string& input);
  std::string ToString() const;

  size_t size() const;
  unsigned char* data();
  const unsigned char* data() const;

  HashValueTag tag;

 private:
  union {
    SHA1HashValue sha1;
    SHA256HashValue sha256;
  } fingerprint;
};

typedef std::vector<HashValue> HashValueVector;


class SHA1HashValueLessThan {
 public:
  bool operator()(const SHA1HashValue& lhs,
                  const SHA1HashValue& rhs) const {
    return memcmp(lhs.data, rhs.data, sizeof(lhs.data)) < 0;
  }
};

class SHA256HashValueLessThan {
 public:
  bool operator()(const SHA256HashValue& lhs,
                  const SHA256HashValue& rhs) const {
    return memcmp(lhs.data, rhs.data, sizeof(lhs.data)) < 0;
  }
};

class HashValuesEqual {
  public:
  explicit HashValuesEqual(const HashValue& fingerprint) :
      fingerprint_(fingerprint) {}

  bool operator()(const HashValue& other) const {
    return fingerprint_.Equals(other);
  }

  const HashValue& fingerprint_;
};


// IsSHA1HashInSortedArray returns true iff |hash| is in |array|, a sorted
// array of SHA1 hashes.
bool IsSHA1HashInSortedArray(const SHA1HashValue& hash,
                             const uint8* array,
                             size_t array_byte_len);

}  // namespace net

#endif  // NET_BASE_HASH_VALUE_H_
