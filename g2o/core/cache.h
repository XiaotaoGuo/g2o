// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, W. Burgard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef G2O_CACHE_HH_
#define G2O_CACHE_HH_

#include <map>
#include <memory>

#include "optimizable_graph.h"
#include "g2o_core_api.h"

namespace g2o {

  class CacheContainer;

  class G2O_CORE_API Cache: public HyperGraph::HyperGraphElement
  {
    public:
      friend class CacheContainer;
      class G2O_CORE_API CacheKey
      {
        public:
          friend class CacheContainer;
          CacheKey();
          CacheKey(const std::string& type_, const ParameterVector& parameters_);

          bool operator<(const CacheKey& c) const;

          const std::string& type() const { return _type;}
          const ParameterVector& parameters() const { return _parameters;}

        protected:
          std::string _type;
          ParameterVector _parameters;
      };

      Cache(CacheContainer* container_ = nullptr,
            const ParameterVector& parameters_ = ParameterVector());

      CacheKey key() const;

      OptimizableGraph::Vertex* vertex() const;
      const ParameterVector& parameters() const;

      void update();

      virtual HyperGraph::HyperGraphElementType elementType() const { return HyperGraph::HGET_CACHE;}

    protected:
      //! redefine this to do the update
      virtual void updateImpl() = 0;

      bool _updateNeeded;
      ParameterVector _parameters;
      CacheContainer* _container;
  };

  class G2O_CORE_API CacheContainer: public std::map<Cache::CacheKey, std::shared_ptr<Cache>>
  {
    public:
      friend OptimizableGraph::Edge;
      explicit CacheContainer(OptimizableGraph::Vertex* vertex_);
      OptimizableGraph::Vertex* vertex() const;
      std::shared_ptr<Cache> findCache(const Cache::CacheKey& key);
      void setUpdateNeeded(bool needUpdate=true);
      void update();
    protected:
      std::shared_ptr<Cache> createCache(const Cache::CacheKey& key);
      OptimizableGraph::Vertex* _vertex;
      bool _updateNeeded;
  };


  template <typename CacheType>
  void OptimizableGraph::Edge::resolveCache(std::shared_ptr<CacheType>& cache,
      const std::shared_ptr<OptimizableGraph::Vertex>& v,
      const std::string& type_,
      const ParameterVector& parameters_)
  {
    CacheContainer* container= v->cacheContainer();
    Cache::CacheKey key(type_, parameters_);
    std::shared_ptr<Cache> c = container->findCache(key);
    if (!c) {
      c = container->createCache(key);
    }
    cache = std::dynamic_pointer_cast<CacheType>(c);
  }

} // end namespace

#endif
