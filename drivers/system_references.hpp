/** MIT License
 * 
 * Copyright (c) 2025 Feb. by Julian Bruin, James Schutte, Richard Kroesen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SYSTEM_REFERENCES_HPP
#define SYSTEM_REFERENCES_HPP

#include <stdint.h>

/* Macro defines: */
#define TRUE 1
#define FALSE 0

#define DEBUG (FALSE)

enum class ADC_FREQUENCIES : uint32_t {
    FS_100hz    = 100,
    FS_1k       = 1'000,
    FS_10k      = 10'000,
    FS_100k     = 100'000,
    FS_MAX_PICO = 500'000,
}; 

#endif /* SYSTEM_REFERENCES_HPP */