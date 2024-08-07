/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#define for_each_idx(i, n) for (i = 0; i < n; i++)

/*
 * the n that this for loop macro can handle is up to SIZE_MAX - 1
 */
#define for_each_idx_back(i, n) \
	for (i = n; i != SIZE_MAX; i--)

#define for_each_idx_from(i, n) for (; i < n; i++)

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
