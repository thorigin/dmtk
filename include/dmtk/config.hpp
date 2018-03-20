/**
 * Copyright (C) Omar Thor, Aurora Hernandez - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by
 *    Omar Thor <omar@thorigin.com>, 2018
 *    Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef DMTK_CONFIG_HPP
#define DMTK_CONFIG_HPP

#define DMTK_NAMESPACE_BEGIN namespace dmtk {
#define DMTK_NAMESPACE_END }

#define DMTK_DETAIL_NAMESPACE_BEGIN namespace dmtk { namespace detail {
#define DMTK_DETAIL_NAMESPACE_END }}

#ifndef DMTK_FP_TYPE
#define DMTK_FP_TYPE double
#endif

using fp_type = DMTK_FP_TYPE;

#endif /* DMTK_CONFIG_HPP */

