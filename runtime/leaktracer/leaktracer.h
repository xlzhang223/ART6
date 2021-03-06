/* -*- mode: c++ -*-
 * Copyright (C) 2015 Yu Hengyang.
 *
 *
 * leaktracer.h
 *
 * History:
 *
 *   Created on 2015-9-5 by Yu Hengyang, hengyyu@gmail.com
 */
#ifndef ART_RUNTIME_LEAKTRACER_LEAKTRACER_H_
#define ART_RUNTIME_LEAKTRACER_LEAKTRACER_H_

#include "ac_defs.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include "../base/macros.h"

namespace art {
  namespace gc {
    class Heap;
  }

  namespace mirror {
    class Class;
  }

    class ArtMethod;

  namespace leaktracer {
    // const int KB = 1024;
    // const int kTLSBufferSize = 16*KB;
    // struct Buffer {
    //   u32 count;
    //   u32 data[1];
    // };
    // const int kBufferLimit = kTLSBufferSize/sizeof(unsigned int) - 1;
    // inline void TLSDestructor(void *tls) {
    //   // called by pthread when a thread is dying
    //   delete tls;
    //   ThreadPool *pool = Runtime::Current()->GetHeap()->GetThreadPool();
    //   pthread_setspecific(pool->GetThreadLocalBufferKey(), NULL);
    // }
    // inline void InitBuffer(Buffer *p) {
    //   p->count = 0;
    // }

    extern bool gLeakTracerIsTracking;

    class LeakTracer {
    public:
      explicit LeakTracer(const char *proc_name);
      ~LeakTracer();

      // create a new instance of LeakTracer
      // return 0 on success, errno on failure
      static int Create(const char *proc_name);
      static LeakTracer* Instance() { return instance_; }


      void NewObject(void *addr, size_t size);
      void DeadObject(void *addr);
      void MoveObject(void *from, void *to);
      void AccessObject(void *addr);

      void GcStarted(bool is_compacting_gc);
      void GcFinished();

      void AppStarted();
      void AppFinished();

      void NewClass(mirror::Class *klass);
      void NewMethodLinked(const std::string& class_name, ArtMethod& method);

    private:
      // returns 0 on success, errno on failure
      int OpenDataFile();
      int OpenClassFile();
      void CloseFile(FILE *& fp);
      void DumpTypeAndMethodInfo();
      // Buffer* GetTls() {
      //   return pthread_getspecific(gc_thread_pool_->GetThreadLocalBufferKey());
      // }

      inline void WriteSafe(void *p, size_t size) {
        fwrite(p, size, 1, data_fp_);
      }

    private:
      struct ClassOrMethod {
        ClassOrMethod(unsigned int d1, unsigned int d2, std::string& str)
          :data1(d1), data2(d2), name(str) {}
        unsigned int data1, data2;
        std::string name;
      };

      FILE *data_fp_;           // object events should write to this file
      FILE *class_fp_;          // class name, method name, code range should write to this file
      std::string proc_name_;
      size_t num_gc_;

      std::unordered_set<u32> acc_sets;
      std::set<mirror::Class*> classes_;
      std::vector<ClassOrMethod> class_meta_info_;
      std::vector<ClassOrMethod> method_meta_info_;
      // ThreadPool *gc_thread_pool_;
      static LeakTracer *instance_;

      DISALLOW_COPY_AND_ASSIGN(LeakTracer);
    };

}  // namespace leaktracer
}  // namespace art

#endif  // ART_RUNTIME_LEAKTRACER_LEAKTRACER_H_

