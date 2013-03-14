/***************************************************************************************************
 *  Copyright 2012 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#ifndef MAIDSAFE_PASSPORT_PASSPORT_H_
#define MAIDSAFE_PASSPORT_PASSPORT_H_

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"
#include "maidsafe/passport/detail/fob.h"
#include "maidsafe/passport/detail/identity_data.h"


namespace maidsafe {

namespace passport {

Mid::name_type MidName(const UserKeyword& keyword, uint32_t pin);

Smid::name_type SmidName(const UserKeyword& keyword, uint32_t pin);

EncryptedSession EncryptSession(const UserKeyword& keyword,
                                uint32_t pin,
                                const UserPassword& password,
                                const NonEmptyString& serialised_session);

EncryptedTmidName EncryptTmidName(const UserPassword& password,
                                  uint32_t pin,
                                  const Tmid::name_type& tmid_name);

Tmid::name_type DecryptTmidName(const UserPassword& password,
                                uint32_t pin,
                                const EncryptedTmidName& encrypted_tmid_name);

NonEmptyString DecryptSession(const UserKeyword& keyword,
                              uint32_t pin,
                              const UserPassword& password,
                              const EncryptedSession& encrypted_session);

NonEmptyString SerialisePmid(const Pmid& pmid);

Pmid ParsePmid(const NonEmptyString& serialised_pmid);

namespace test { class PassportTest; }


class Passport {
 public:
  Passport();
  void CreateFobs();
  void ConfirmFobs();

  NonEmptyString Serialise();
  void Parse(const NonEmptyString& serialised_passport);

  template<typename FobType>
  FobType Get(bool confirmed);

  // Selectable Fob (aka ANMPID & MPID)
  template<typename FobType>
  FobType GetSelectableFob(bool confirmed, const NonEmptyString& chosen_name);
  void CreateSelectableFobPair(const NonEmptyString& chosen_name);
  void ConfirmSelectableFobPair(const NonEmptyString& chosen_name);
  void DeleteSelectableFobPair(const NonEmptyString& chosen_name);
  std::vector<NonEmptyString> GetSelectableFobNameList(bool confirmed);

  friend class test::PassportTest;

 private:
  Passport(const Passport&);
  Passport& operator=(const Passport&);

  struct Fobs {
    Fobs() : anmid(), ansmid(), antmid(), anmaid(), maid(), pmid() {}
    Fobs(Fobs&& other)
        : anmid(std::move(other.anmid)),
          ansmid(std::move(other.ansmid)),
          antmid(std::move(other.antmid)),
          anmaid(std::move(other.anmaid)),
          maid(std::move(other.maid)),
          pmid(std::move(other.pmid)) {}
    Fobs& operator=(Fobs&& other) {
      anmid = std::move(other.anmid);
      ansmid = std::move(other.ansmid);
      antmid = std::move(other.antmid);
      anmaid = std::move(other.anmaid);
      maid = std::move(other.maid);
      pmid = std::move(other.pmid);
      return *this;
    }
    std::unique_ptr<Anmid> anmid;
    std::unique_ptr<Ansmid> ansmid;
    std::unique_ptr<Antmid> antmid;
    std::unique_ptr<Anmaid> anmaid;
    std::unique_ptr<Maid> maid;
    std::unique_ptr<Pmid> pmid;

   private:
    Fobs(const Fobs&);
    Fobs& operator=(const Fobs&);
  };

  struct SelectableFobPair {
    SelectableFobPair() : anmpid(), mpid() {}
    SelectableFobPair(SelectableFobPair&& other)
        : anmpid(std::move(other.anmpid)),
          mpid(std::move(other.mpid)) {}
    SelectableFobPair& operator=(SelectableFobPair&& other) {
      anmpid = std::move(other.anmpid);
      mpid = std::move(other.mpid);
      return *this;
    }
    std::unique_ptr<Anmpid> anmpid;
    std::unique_ptr<Mpid> mpid;

   private:
#ifdef MAIDSAFE_WIN32
    SelectableFobPair(const SelectableFobPair&);
#else
    SelectableFobPair(const SelectableFobPair&) = delete;
#endif
    SelectableFobPair& operator=(const SelectableFobPair&);
  };

  bool NoFobsNull(bool confirmed);
  template<typename FobType>
  FobType GetFromSelectableFobPair(bool confirmed, const SelectableFobPair& selectable_fob_pair);

  Fobs pending_fobs_, confirmed_fobs_;
  std::map<NonEmptyString, SelectableFobPair> pending_selectable_fobs_, confirmed_selectable_fobs_;
  std::mutex fobs_mutex_, selectable_mutex_;
};

}  // namespace passport

}  // namespace maidsafe

#include "maidsafe/passport/detail/passport-inl.h"

#endif  // MAIDSAFE_PASSPORT_PASSPORT_H_
