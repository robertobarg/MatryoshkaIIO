///
/// src/algs/Matryoshka.h
///
/// This class implements a matryoshka algorithm for the resolution of image-processing instances of the optimal transport 
/// The algorithm solves successive transportation problems by means of the iterated inside-out algorithm by R. Bargetto, F. Della Croce, R. Scatamacchia
/// Iterated inside-out algorithm: https://arxiv.org/pdf/2302.10826.pdf
/// 
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// -------------------
///     The implementation of method 'Matryoshka::init_sol(..)' is derived from a routine written by Rosario Scatamacchia
///     DIGEP, Politecnico di Torino
///     Copyright 2024 by Rosario Scatamacchia
///     All rights reserved
/// -------------------
///
/// Copyright 2024 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
/// All rights reserved
///


#ifndef MATRYOSHKA_H
#define MATRYOSHKA_H

#include "TpInstance.h"
#include "TSimplexDatastructs.h"
#include "optresult.h"
#include "optcfg.h"


class Matryoshka
{
public:
    Matryoshka(const unsigned int lvls, const std::shared_ptr<TpInstance::TProblemData>& pdata_ptr, bool verbose = false);
    ~Matryoshka();
    
    optresult run(const optcfg& opt_config);
    
private:
    std::shared_ptr<TpInstance::TProblemData> reduceProbSize(const std::shared_ptr<TpInstance::TProblemData>&);
    optresult
    init_sol(const std::vector<std::pair<NodeArcIdType, NodeArcIdType>>& solution,
             const std::shared_ptr<TpInstance::TProblemData>& tpd_sptr,
             const std::shared_ptr<TpInstance::TProblemData>& twotpd_sptr,
             std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>>& sol_arcs_sptr,
             std::shared_ptr<TSimplexData::ts_sol>& sol_sptr);
    
    std::shared_ptr<TpInstance::TProblemData> tpdata_l0_ptr = nullptr;
    unsigned int levels;
    bool verbose;
};


#endif //  MATRYOSHKA_H
