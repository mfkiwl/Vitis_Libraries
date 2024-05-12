/*
 * Copyright (C) 2019-2022, Xilinx, Inc.
 * Copyright (C) 2022-2024, Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
static constexpr int twiddle_table_size = 1024;
alignas(32) const cint16 twiddle_coarse[twiddle_table_size] = {
    {23170, 0},       {23169, 142},     {23168, 284},     {23166, 426},     {23163, 569},     {23159, 711},
    {23154, 853},     {23148, 995},     {23142, 1137},    {23134, 1279},    {23126, 1421},    {23117, 1563},
    {23107, 1704},    {23096, 1846},    {23084, 1988},    {23072, 2130},    {23058, 2271},    {23044, 2412},
    {23029, 2554},    {23012, 2695},    {22996, 2836},    {22978, 2977},    {22959, 3118},    {22939, 3259},
    {22919, 3400},    {22898, 3540},    {22876, 3681},    {22853, 3821},    {22829, 3961},    {22804, 4101},
    {22778, 4241},    {22752, 4381},    {22725, 4520},    {22696, 4660},    {22667, 4799},    {22638, 4938},
    {22607, 5077},    {22575, 5215},    {22543, 5354},    {22510, 5492},    {22475, 5630},    {22440, 5768},
    {22405, 5905},    {22368, 6043},    {22330, 6180},    {22292, 6317},    {22253, 6453},    {22213, 6590},
    {22172, 6726},    {22130, 6862},    {22088, 6997},    {22045, 7133},    {22000, 7268},    {21955, 7403},
    {21910, 7537},    {21863, 7672},    {21815, 7806},    {21767, 7939},    {21718, 8073},    {21668, 8206},
    {21617, 8339},    {21566, 8471},    {21513, 8603},    {21460, 8735},    {21406, 8867},    {21351, 8998},
    {21296, 9129},    {21239, 9259},    {21182, 9389},    {21124, 9519},    {21065, 9649},    {21006, 9778},
    {20945, 9906},    {20884, 10035},   {20822, 10163},   {20759, 10290},   {20696, 10417},   {20631, 10544},
    {20566, 10671},   {20501, 10797},   {20434, 10922},   {20367, 11047},   {20298, 11172},   {20229, 11296},
    {20160, 11420},   {20089, 11544},   {20018, 11667},   {19946, 11789},   {19873, 11912},   {19800, 12033},
    {19726, 12155},   {19651, 12275},   {19575, 12396},   {19499, 12516},   {19421, 12635},   {19344, 12754},
    {19265, 12872},   {19186, 12990},   {19106, 13108},   {19025, 13225},   {18943, 13341},   {18861, 13457},
    {18778, 13573},   {18694, 13688},   {18610, 13802},   {18525, 13916},   {18439, 14030},   {18353, 14142},
    {18266, 14255},   {18178, 14367},   {18090, 14478},   {18000, 14589},   {17910, 14699},   {17820, 14808},
    {17729, 14917},   {17637, 15026},   {17544, 15134},   {17451, 15241},   {17357, 15348},   {17263, 15454},
    {17168, 15560},   {17072, 15665},   {16975, 15769},   {16878, 15873},   {16781, 15976},   {16682, 16079},
    {16583, 16181},   {16484, 16283},   {16384, 16383},   {16283, 16484},   {16181, 16583},   {16079, 16682},
    {15976, 16781},   {15873, 16878},   {15769, 16975},   {15665, 17072},   {15560, 17168},   {15454, 17263},
    {15348, 17357},   {15241, 17451},   {15134, 17544},   {15026, 17637},   {14917, 17729},   {14808, 17820},
    {14699, 17910},   {14589, 18000},   {14478, 18090},   {14367, 18178},   {14255, 18266},   {14142, 18353},
    {14030, 18439},   {13916, 18525},   {13802, 18610},   {13688, 18694},   {13573, 18778},   {13457, 18861},
    {13341, 18943},   {13225, 19025},   {13108, 19106},   {12990, 19186},   {12872, 19265},   {12754, 19344},
    {12635, 19421},   {12516, 19499},   {12396, 19575},   {12275, 19651},   {12155, 19726},   {12033, 19800},
    {11912, 19873},   {11789, 19946},   {11667, 20018},   {11544, 20089},   {11420, 20160},   {11296, 20229},
    {11172, 20298},   {11047, 20367},   {10922, 20434},   {10797, 20501},   {10671, 20566},   {10544, 20631},
    {10417, 20696},   {10290, 20759},   {10163, 20822},   {10035, 20884},   {9906, 20945},    {9778, 21006},
    {9649, 21065},    {9519, 21124},    {9389, 21182},    {9259, 21239},    {9129, 21296},    {8998, 21351},
    {8867, 21406},    {8735, 21460},    {8603, 21513},    {8471, 21566},    {8339, 21617},    {8206, 21668},
    {8073, 21718},    {7939, 21767},    {7806, 21815},    {7672, 21863},    {7537, 21910},    {7403, 21955},
    {7268, 22000},    {7133, 22045},    {6997, 22088},    {6862, 22130},    {6726, 22172},    {6590, 22213},
    {6453, 22253},    {6317, 22292},    {6180, 22330},    {6043, 22368},    {5905, 22405},    {5768, 22440},
    {5630, 22475},    {5492, 22510},    {5354, 22543},    {5215, 22575},    {5077, 22607},    {4938, 22638},
    {4799, 22667},    {4660, 22696},    {4520, 22725},    {4381, 22752},    {4241, 22778},    {4101, 22804},
    {3961, 22829},    {3821, 22853},    {3681, 22876},    {3540, 22898},    {3400, 22919},    {3259, 22939},
    {3118, 22959},    {2977, 22978},    {2836, 22996},    {2695, 23012},    {2554, 23029},    {2412, 23044},
    {2271, 23058},    {2130, 23072},    {1988, 23084},    {1846, 23096},    {1704, 23107},    {1563, 23117},
    {1421, 23126},    {1279, 23134},    {1137, 23142},    {995, 23148},     {853, 23154},     {711, 23159},
    {569, 23163},     {426, 23166},     {284, 23168},     {142, 23169},     {0, 23170},       {-142, 23169},
    {-284, 23168},    {-426, 23166},    {-569, 23163},    {-711, 23159},    {-853, 23154},    {-995, 23148},
    {-1137, 23142},   {-1279, 23134},   {-1421, 23126},   {-1563, 23117},   {-1704, 23107},   {-1846, 23096},
    {-1988, 23084},   {-2130, 23072},   {-2271, 23058},   {-2412, 23044},   {-2554, 23029},   {-2695, 23012},
    {-2836, 22996},   {-2977, 22978},   {-3118, 22959},   {-3259, 22939},   {-3400, 22919},   {-3540, 22898},
    {-3681, 22876},   {-3821, 22853},   {-3961, 22829},   {-4101, 22804},   {-4241, 22778},   {-4381, 22752},
    {-4520, 22725},   {-4660, 22696},   {-4799, 22667},   {-4938, 22638},   {-5077, 22607},   {-5215, 22575},
    {-5354, 22543},   {-5492, 22510},   {-5630, 22475},   {-5768, 22440},   {-5905, 22405},   {-6043, 22368},
    {-6180, 22330},   {-6317, 22292},   {-6453, 22253},   {-6590, 22213},   {-6726, 22172},   {-6862, 22130},
    {-6997, 22088},   {-7133, 22045},   {-7268, 22000},   {-7403, 21955},   {-7537, 21910},   {-7672, 21863},
    {-7806, 21815},   {-7939, 21767},   {-8073, 21718},   {-8206, 21668},   {-8339, 21617},   {-8471, 21566},
    {-8603, 21513},   {-8735, 21460},   {-8867, 21406},   {-8998, 21351},   {-9129, 21296},   {-9259, 21239},
    {-9389, 21182},   {-9519, 21124},   {-9649, 21065},   {-9778, 21006},   {-9906, 20945},   {-10035, 20884},
    {-10163, 20822},  {-10290, 20759},  {-10417, 20696},  {-10544, 20631},  {-10671, 20566},  {-10797, 20501},
    {-10922, 20434},  {-11047, 20367},  {-11172, 20298},  {-11296, 20229},  {-11420, 20160},  {-11544, 20089},
    {-11667, 20018},  {-11789, 19946},  {-11912, 19873},  {-12033, 19800},  {-12155, 19726},  {-12275, 19651},
    {-12396, 19575},  {-12516, 19499},  {-12635, 19421},  {-12754, 19344},  {-12872, 19265},  {-12990, 19186},
    {-13108, 19106},  {-13225, 19025},  {-13341, 18943},  {-13457, 18861},  {-13573, 18778},  {-13688, 18694},
    {-13802, 18610},  {-13916, 18525},  {-14030, 18439},  {-14142, 18353},  {-14255, 18266},  {-14367, 18178},
    {-14478, 18090},  {-14589, 18000},  {-14699, 17910},  {-14808, 17820},  {-14917, 17729},  {-15026, 17637},
    {-15134, 17544},  {-15241, 17451},  {-15348, 17357},  {-15454, 17263},  {-15560, 17168},  {-15665, 17072},
    {-15769, 16975},  {-15873, 16878},  {-15976, 16781},  {-16079, 16682},  {-16181, 16583},  {-16283, 16484},
    {-16383, 16384},  {-16484, 16283},  {-16583, 16181},  {-16682, 16079},  {-16781, 15976},  {-16878, 15873},
    {-16975, 15769},  {-17072, 15665},  {-17168, 15560},  {-17263, 15454},  {-17357, 15348},  {-17451, 15241},
    {-17544, 15134},  {-17637, 15026},  {-17729, 14917},  {-17820, 14808},  {-17910, 14699},  {-18000, 14589},
    {-18090, 14478},  {-18178, 14367},  {-18266, 14255},  {-18353, 14142},  {-18439, 14030},  {-18525, 13916},
    {-18610, 13802},  {-18694, 13688},  {-18778, 13573},  {-18861, 13457},  {-18943, 13341},  {-19025, 13225},
    {-19106, 13108},  {-19186, 12990},  {-19265, 12872},  {-19344, 12754},  {-19421, 12635},  {-19499, 12516},
    {-19575, 12396},  {-19651, 12275},  {-19726, 12155},  {-19800, 12033},  {-19873, 11912},  {-19946, 11789},
    {-20018, 11667},  {-20089, 11544},  {-20160, 11420},  {-20229, 11296},  {-20298, 11172},  {-20367, 11047},
    {-20434, 10922},  {-20501, 10797},  {-20566, 10671},  {-20631, 10544},  {-20696, 10417},  {-20759, 10290},
    {-20822, 10163},  {-20884, 10035},  {-20945, 9906},   {-21006, 9778},   {-21065, 9649},   {-21124, 9519},
    {-21182, 9389},   {-21239, 9259},   {-21296, 9129},   {-21351, 8998},   {-21406, 8867},   {-21460, 8735},
    {-21513, 8603},   {-21566, 8471},   {-21617, 8339},   {-21668, 8206},   {-21718, 8073},   {-21767, 7939},
    {-21815, 7806},   {-21863, 7672},   {-21910, 7537},   {-21955, 7403},   {-22000, 7268},   {-22045, 7133},
    {-22088, 6997},   {-22130, 6862},   {-22172, 6726},   {-22213, 6590},   {-22253, 6453},   {-22292, 6317},
    {-22330, 6180},   {-22368, 6043},   {-22405, 5905},   {-22440, 5768},   {-22475, 5630},   {-22510, 5492},
    {-22543, 5354},   {-22575, 5215},   {-22607, 5077},   {-22638, 4938},   {-22667, 4799},   {-22696, 4660},
    {-22725, 4520},   {-22752, 4381},   {-22778, 4241},   {-22804, 4101},   {-22829, 3961},   {-22853, 3821},
    {-22876, 3681},   {-22898, 3540},   {-22919, 3400},   {-22939, 3259},   {-22959, 3118},   {-22978, 2977},
    {-22996, 2836},   {-23012, 2695},   {-23029, 2554},   {-23044, 2412},   {-23058, 2271},   {-23072, 2130},
    {-23084, 1988},   {-23096, 1846},   {-23107, 1704},   {-23117, 1563},   {-23126, 1421},   {-23134, 1279},
    {-23142, 1137},   {-23148, 995},    {-23154, 853},    {-23159, 711},    {-23163, 569},    {-23166, 426},
    {-23168, 284},    {-23169, 142},    {-23170, 0},      {-23169, -142},   {-23168, -284},   {-23166, -426},
    {-23163, -569},   {-23159, -711},   {-23154, -853},   {-23148, -995},   {-23142, -1137},  {-23134, -1279},
    {-23126, -1421},  {-23117, -1563},  {-23107, -1704},  {-23096, -1846},  {-23084, -1988},  {-23072, -2130},
    {-23058, -2271},  {-23044, -2412},  {-23029, -2554},  {-23012, -2695},  {-22996, -2836},  {-22978, -2977},
    {-22959, -3118},  {-22939, -3259},  {-22919, -3400},  {-22898, -3540},  {-22876, -3681},  {-22853, -3821},
    {-22829, -3961},  {-22804, -4101},  {-22778, -4241},  {-22752, -4381},  {-22725, -4520},  {-22696, -4660},
    {-22667, -4799},  {-22638, -4938},  {-22607, -5077},  {-22575, -5215},  {-22543, -5354},  {-22510, -5492},
    {-22475, -5630},  {-22440, -5768},  {-22405, -5905},  {-22368, -6043},  {-22330, -6180},  {-22292, -6317},
    {-22253, -6453},  {-22213, -6590},  {-22172, -6726},  {-22130, -6862},  {-22088, -6997},  {-22045, -7133},
    {-22000, -7268},  {-21955, -7403},  {-21910, -7537},  {-21863, -7672},  {-21815, -7806},  {-21767, -7939},
    {-21718, -8073},  {-21668, -8206},  {-21617, -8339},  {-21566, -8471},  {-21513, -8603},  {-21460, -8735},
    {-21406, -8867},  {-21351, -8998},  {-21296, -9129},  {-21239, -9259},  {-21182, -9389},  {-21124, -9519},
    {-21065, -9649},  {-21006, -9778},  {-20945, -9906},  {-20884, -10035}, {-20822, -10163}, {-20759, -10290},
    {-20696, -10417}, {-20631, -10544}, {-20566, -10671}, {-20501, -10797}, {-20434, -10922}, {-20367, -11047},
    {-20298, -11172}, {-20229, -11296}, {-20160, -11420}, {-20089, -11544}, {-20018, -11667}, {-19946, -11789},
    {-19873, -11912}, {-19800, -12033}, {-19726, -12155}, {-19651, -12275}, {-19575, -12396}, {-19499, -12516},
    {-19421, -12635}, {-19344, -12754}, {-19265, -12872}, {-19186, -12990}, {-19106, -13108}, {-19025, -13225},
    {-18943, -13341}, {-18861, -13457}, {-18778, -13573}, {-18694, -13688}, {-18610, -13802}, {-18525, -13916},
    {-18439, -14030}, {-18353, -14142}, {-18266, -14255}, {-18178, -14367}, {-18090, -14478}, {-18000, -14589},
    {-17910, -14699}, {-17820, -14808}, {-17729, -14917}, {-17637, -15026}, {-17544, -15134}, {-17451, -15241},
    {-17357, -15348}, {-17263, -15454}, {-17168, -15560}, {-17072, -15665}, {-16975, -15769}, {-16878, -15873},
    {-16781, -15976}, {-16682, -16079}, {-16583, -16181}, {-16484, -16283}, {-16384, -16383}, {-16283, -16484},
    {-16181, -16583}, {-16079, -16682}, {-15976, -16781}, {-15873, -16878}, {-15769, -16975}, {-15665, -17072},
    {-15560, -17168}, {-15454, -17263}, {-15348, -17357}, {-15241, -17451}, {-15134, -17544}, {-15026, -17637},
    {-14917, -17729}, {-14808, -17820}, {-14699, -17910}, {-14589, -18000}, {-14478, -18090}, {-14367, -18178},
    {-14255, -18266}, {-14142, -18353}, {-14030, -18439}, {-13916, -18525}, {-13802, -18610}, {-13688, -18694},
    {-13573, -18778}, {-13457, -18861}, {-13341, -18943}, {-13225, -19025}, {-13108, -19106}, {-12990, -19186},
    {-12872, -19265}, {-12754, -19344}, {-12635, -19421}, {-12516, -19499}, {-12396, -19575}, {-12275, -19651},
    {-12155, -19726}, {-12033, -19800}, {-11912, -19873}, {-11789, -19946}, {-11667, -20018}, {-11544, -20089},
    {-11420, -20160}, {-11296, -20229}, {-11172, -20298}, {-11047, -20367}, {-10922, -20434}, {-10797, -20501},
    {-10671, -20566}, {-10544, -20631}, {-10417, -20696}, {-10290, -20759}, {-10163, -20822}, {-10035, -20884},
    {-9906, -20945},  {-9778, -21006},  {-9649, -21065},  {-9519, -21124},  {-9389, -21182},  {-9259, -21239},
    {-9129, -21296},  {-8998, -21351},  {-8867, -21406},  {-8735, -21460},  {-8603, -21513},  {-8471, -21566},
    {-8339, -21617},  {-8206, -21668},  {-8073, -21718},  {-7939, -21767},  {-7806, -21815},  {-7672, -21863},
    {-7537, -21910},  {-7403, -21955},  {-7268, -22000},  {-7133, -22045},  {-6997, -22088},  {-6862, -22130},
    {-6726, -22172},  {-6590, -22213},  {-6453, -22253},  {-6317, -22292},  {-6180, -22330},  {-6043, -22368},
    {-5905, -22405},  {-5768, -22440},  {-5630, -22475},  {-5492, -22510},  {-5354, -22543},  {-5215, -22575},
    {-5077, -22607},  {-4938, -22638},  {-4799, -22667},  {-4660, -22696},  {-4520, -22725},  {-4381, -22752},
    {-4241, -22778},  {-4101, -22804},  {-3961, -22829},  {-3821, -22853},  {-3681, -22876},  {-3540, -22898},
    {-3400, -22919},  {-3259, -22939},  {-3118, -22959},  {-2977, -22978},  {-2836, -22996},  {-2695, -23012},
    {-2554, -23029},  {-2412, -23044},  {-2271, -23058},  {-2130, -23072},  {-1988, -23084},  {-1846, -23096},
    {-1704, -23107},  {-1563, -23117},  {-1421, -23126},  {-1279, -23134},  {-1137, -23142},  {-995, -23148},
    {-853, -23154},   {-711, -23159},   {-569, -23163},   {-426, -23166},   {-284, -23168},   {-142, -23169},
    {0, -23170},      {142, -23169},    {284, -23168},    {426, -23166},    {569, -23163},    {711, -23159},
    {853, -23154},    {995, -23148},    {1137, -23142},   {1279, -23134},   {1421, -23126},   {1563, -23117},
    {1704, -23107},   {1846, -23096},   {1988, -23084},   {2130, -23072},   {2271, -23058},   {2412, -23044},
    {2554, -23029},   {2695, -23012},   {2836, -22996},   {2977, -22978},   {3118, -22959},   {3259, -22939},
    {3400, -22919},   {3540, -22898},   {3681, -22876},   {3821, -22853},   {3961, -22829},   {4101, -22804},
    {4241, -22778},   {4381, -22752},   {4520, -22725},   {4660, -22696},   {4799, -22667},   {4938, -22638},
    {5077, -22607},   {5215, -22575},   {5354, -22543},   {5492, -22510},   {5630, -22475},   {5768, -22440},
    {5905, -22405},   {6043, -22368},   {6180, -22330},   {6317, -22292},   {6453, -22253},   {6590, -22213},
    {6726, -22172},   {6862, -22130},   {6997, -22088},   {7133, -22045},   {7268, -22000},   {7403, -21955},
    {7537, -21910},   {7672, -21863},   {7806, -21815},   {7939, -21767},   {8073, -21718},   {8206, -21668},
    {8339, -21617},   {8471, -21566},   {8603, -21513},   {8735, -21460},   {8867, -21406},   {8998, -21351},
    {9129, -21296},   {9259, -21239},   {9389, -21182},   {9519, -21124},   {9649, -21065},   {9778, -21006},
    {9906, -20945},   {10035, -20884},  {10163, -20822},  {10290, -20759},  {10417, -20696},  {10544, -20631},
    {10671, -20566},  {10797, -20501},  {10922, -20434},  {11047, -20367},  {11172, -20298},  {11296, -20229},
    {11420, -20160},  {11544, -20089},  {11667, -20018},  {11789, -19946},  {11912, -19873},  {12033, -19800},
    {12155, -19726},  {12275, -19651},  {12396, -19575},  {12516, -19499},  {12635, -19421},  {12754, -19344},
    {12872, -19265},  {12990, -19186},  {13108, -19106},  {13225, -19025},  {13341, -18943},  {13457, -18861},
    {13573, -18778},  {13688, -18694},  {13802, -18610},  {13916, -18525},  {14030, -18439},  {14142, -18353},
    {14255, -18266},  {14367, -18178},  {14478, -18090},  {14589, -18000},  {14699, -17910},  {14808, -17820},
    {14917, -17729},  {15026, -17637},  {15134, -17544},  {15241, -17451},  {15348, -17357},  {15454, -17263},
    {15560, -17168},  {15665, -17072},  {15769, -16975},  {15873, -16878},  {15976, -16781},  {16079, -16682},
    {16181, -16583},  {16283, -16484},  {16383, -16384},  {16484, -16283},  {16583, -16181},  {16682, -16079},
    {16781, -15976},  {16878, -15873},  {16975, -15769},  {17072, -15665},  {17168, -15560},  {17263, -15454},
    {17357, -15348},  {17451, -15241},  {17544, -15134},  {17637, -15026},  {17729, -14917},  {17820, -14808},
    {17910, -14699},  {18000, -14589},  {18090, -14478},  {18178, -14367},  {18266, -14255},  {18353, -14142},
    {18439, -14030},  {18525, -13916},  {18610, -13802},  {18694, -13688},  {18778, -13573},  {18861, -13457},
    {18943, -13341},  {19025, -13225},  {19106, -13108},  {19186, -12990},  {19265, -12872},  {19344, -12754},
    {19421, -12635},  {19499, -12516},  {19575, -12396},  {19651, -12275},  {19726, -12155},  {19800, -12033},
    {19873, -11912},  {19946, -11789},  {20018, -11667},  {20089, -11544},  {20160, -11420},  {20229, -11296},
    {20298, -11172},  {20367, -11047},  {20434, -10922},  {20501, -10797},  {20566, -10671},  {20631, -10544},
    {20696, -10417},  {20759, -10290},  {20822, -10163},  {20884, -10035},  {20945, -9906},   {21006, -9778},
    {21065, -9649},   {21124, -9519},   {21182, -9389},   {21239, -9259},   {21296, -9129},   {21351, -8998},
    {21406, -8867},   {21460, -8735},   {21513, -8603},   {21566, -8471},   {21617, -8339},   {21668, -8206},
    {21718, -8073},   {21767, -7939},   {21815, -7806},   {21863, -7672},   {21910, -7537},   {21955, -7403},
    {22000, -7268},   {22045, -7133},   {22088, -6997},   {22130, -6862},   {22172, -6726},   {22213, -6590},
    {22253, -6453},   {22292, -6317},   {22330, -6180},   {22368, -6043},   {22405, -5905},   {22440, -5768},
    {22475, -5630},   {22510, -5492},   {22543, -5354},   {22575, -5215},   {22607, -5077},   {22638, -4938},
    {22667, -4799},   {22696, -4660},   {22725, -4520},   {22752, -4381},   {22778, -4241},   {22804, -4101},
    {22829, -3961},   {22853, -3821},   {22876, -3681},   {22898, -3540},   {22919, -3400},   {22939, -3259},
    {22959, -3118},   {22978, -2977},   {22996, -2836},   {23012, -2695},   {23029, -2554},   {23044, -2412},
    {23058, -2271},   {23072, -2130},   {23084, -1988},   {23096, -1846},   {23107, -1704},   {23117, -1563},
    {23126, -1421},   {23134, -1279},   {23142, -1137},   {23148, -995},    {23154, -853},    {23159, -711},
    {23163, -569},    {23166, -426},    {23168, -284},    {23169, -142}};

alignas(32) const cint16 twiddle_fine[twiddle_table_size] = {
    {23170, 0},   {23170, 0},   {23170, 0},   {23170, 0},   {23170, 1},   {23170, 1},   {23170, 1},   {23170, 1},
    {23170, 1},   {23170, 1},   {23170, 1},   {23170, 2},   {23170, 2},   {23170, 2},   {23170, 2},   {23170, 2},
    {23170, 2},   {23170, 2},   {23170, 2},   {23170, 3},   {23170, 3},   {23170, 3},   {23170, 3},   {23170, 3},
    {23170, 3},   {23170, 3},   {23170, 4},   {23170, 4},   {23170, 4},   {23170, 4},   {23170, 4},   {23170, 4},
    {23170, 4},   {23170, 5},   {23170, 5},   {23170, 5},   {23170, 5},   {23170, 5},   {23170, 5},   {23170, 5},
    {23170, 6},   {23170, 6},   {23170, 6},   {23170, 6},   {23170, 6},   {23170, 6},   {23170, 6},   {23170, 7},
    {23170, 7},   {23170, 7},   {23170, 7},   {23170, 7},   {23170, 7},   {23170, 7},   {23170, 7},   {23170, 8},
    {23170, 8},   {23170, 8},   {23170, 8},   {23170, 8},   {23170, 8},   {23170, 8},   {23170, 9},   {23170, 9},
    {23170, 9},   {23170, 9},   {23170, 9},   {23170, 9},   {23170, 9},   {23170, 10},  {23170, 10},  {23170, 10},
    {23170, 10},  {23170, 10},  {23170, 10},  {23170, 10},  {23170, 11},  {23170, 11},  {23170, 11},  {23170, 11},
    {23170, 11},  {23170, 11},  {23170, 11},  {23170, 12},  {23170, 12},  {23170, 12},  {23170, 12},  {23170, 12},
    {23170, 12},  {23170, 12},  {23170, 12},  {23170, 13},  {23170, 13},  {23170, 13},  {23170, 13},  {23170, 13},
    {23170, 13},  {23170, 13},  {23170, 14},  {23170, 14},  {23170, 14},  {23170, 14},  {23170, 14},  {23170, 14},
    {23170, 14},  {23170, 15},  {23170, 15},  {23170, 15},  {23170, 15},  {23170, 15},  {23170, 15},  {23170, 15},
    {23170, 16},  {23170, 16},  {23170, 16},  {23170, 16},  {23170, 16},  {23170, 16},  {23170, 16},  {23170, 17},
    {23170, 17},  {23170, 17},  {23170, 17},  {23170, 17},  {23170, 17},  {23170, 17},  {23170, 17},  {23170, 18},
    {23170, 18},  {23170, 18},  {23170, 18},  {23170, 18},  {23170, 18},  {23170, 18},  {23170, 19},  {23170, 19},
    {23170, 19},  {23170, 19},  {23170, 19},  {23170, 19},  {23170, 19},  {23170, 20},  {23170, 20},  {23170, 20},
    {23170, 20},  {23170, 20},  {23170, 20},  {23170, 20},  {23170, 21},  {23170, 21},  {23170, 21},  {23170, 21},
    {23170, 21},  {23170, 21},  {23170, 21},  {23170, 22},  {23170, 22},  {23170, 22},  {23170, 22},  {23170, 22},
    {23170, 22},  {23170, 22},  {23170, 22},  {23170, 23},  {23170, 23},  {23170, 23},  {23170, 23},  {23170, 23},
    {23170, 23},  {23170, 23},  {23170, 24},  {23170, 24},  {23170, 24},  {23170, 24},  {23170, 24},  {23170, 24},
    {23170, 24},  {23170, 25},  {23170, 25},  {23170, 25},  {23170, 25},  {23170, 25},  {23170, 25},  {23170, 25},
    {23170, 26},  {23170, 26},  {23170, 26},  {23170, 26},  {23170, 26},  {23170, 26},  {23170, 26},  {23170, 27},
    {23170, 27},  {23170, 27},  {23170, 27},  {23170, 27},  {23170, 27},  {23170, 27},  {23170, 27},  {23170, 28},
    {23170, 28},  {23170, 28},  {23170, 28},  {23170, 28},  {23170, 28},  {23170, 28},  {23170, 29},  {23170, 29},
    {23170, 29},  {23170, 29},  {23170, 29},  {23170, 29},  {23170, 29},  {23170, 30},  {23170, 30},  {23170, 30},
    {23170, 30},  {23170, 30},  {23170, 30},  {23170, 30},  {23170, 31},  {23170, 31},  {23170, 31},  {23170, 31},
    {23170, 31},  {23170, 31},  {23170, 31},  {23170, 32},  {23170, 32},  {23170, 32},  {23170, 32},  {23170, 32},
    {23170, 32},  {23170, 32},  {23170, 32},  {23170, 33},  {23170, 33},  {23170, 33},  {23170, 33},  {23170, 33},
    {23170, 33},  {23170, 33},  {23170, 34},  {23170, 34},  {23170, 34},  {23170, 34},  {23170, 34},  {23170, 34},
    {23170, 34},  {23170, 35},  {23170, 35},  {23170, 35},  {23170, 35},  {23170, 35},  {23170, 35},  {23170, 35},
    {23170, 36},  {23170, 36},  {23170, 36},  {23170, 36},  {23170, 36},  {23170, 36},  {23170, 36},  {23170, 37},
    {23170, 37},  {23170, 37},  {23170, 37},  {23170, 37},  {23170, 37},  {23170, 37},  {23170, 37},  {23170, 38},
    {23170, 38},  {23170, 38},  {23170, 38},  {23170, 38},  {23170, 38},  {23170, 38},  {23170, 39},  {23170, 39},
    {23170, 39},  {23170, 39},  {23170, 39},  {23170, 39},  {23170, 39},  {23170, 40},  {23170, 40},  {23170, 40},
    {23170, 40},  {23170, 40},  {23170, 40},  {23170, 40},  {23170, 41},  {23170, 41},  {23170, 41},  {23170, 41},
    {23170, 41},  {23170, 41},  {23170, 41},  {23170, 42},  {23170, 42},  {23170, 42},  {23170, 42},  {23170, 42},
    {23170, 42},  {23170, 42},  {23170, 42},  {23170, 43},  {23170, 43},  {23170, 43},  {23170, 43},  {23170, 43},
    {23170, 43},  {23170, 43},  {23170, 44},  {23170, 44},  {23170, 44},  {23170, 44},  {23170, 44},  {23170, 44},
    {23170, 44},  {23170, 45},  {23170, 45},  {23170, 45},  {23170, 45},  {23170, 45},  {23170, 45},  {23170, 45},
    {23170, 46},  {23170, 46},  {23170, 46},  {23170, 46},  {23170, 46},  {23170, 46},  {23170, 46},  {23170, 47},
    {23170, 47},  {23170, 47},  {23170, 47},  {23170, 47},  {23170, 47},  {23170, 47},  {23170, 47},  {23170, 48},
    {23170, 48},  {23170, 48},  {23170, 48},  {23170, 48},  {23170, 48},  {23170, 48},  {23170, 49},  {23170, 49},
    {23170, 49},  {23170, 49},  {23170, 49},  {23170, 49},  {23170, 49},  {23170, 50},  {23170, 50},  {23170, 50},
    {23170, 50},  {23170, 50},  {23170, 50},  {23170, 50},  {23170, 51},  {23170, 51},  {23170, 51},  {23170, 51},
    {23170, 51},  {23170, 51},  {23170, 51},  {23170, 52},  {23170, 52},  {23170, 52},  {23170, 52},  {23170, 52},
    {23170, 52},  {23170, 52},  {23170, 52},  {23170, 53},  {23170, 53},  {23170, 53},  {23170, 53},  {23170, 53},
    {23170, 53},  {23170, 53},  {23170, 54},  {23170, 54},  {23170, 54},  {23170, 54},  {23170, 54},  {23170, 54},
    {23170, 54},  {23170, 55},  {23170, 55},  {23170, 55},  {23170, 55},  {23170, 55},  {23170, 55},  {23170, 55},
    {23170, 56},  {23170, 56},  {23170, 56},  {23170, 56},  {23170, 56},  {23170, 56},  {23170, 56},  {23170, 57},
    {23170, 57},  {23170, 57},  {23170, 57},  {23170, 57},  {23170, 57},  {23170, 57},  {23170, 57},  {23170, 58},
    {23170, 58},  {23170, 58},  {23170, 58},  {23170, 58},  {23170, 58},  {23170, 58},  {23170, 59},  {23170, 59},
    {23170, 59},  {23170, 59},  {23170, 59},  {23170, 59},  {23170, 59},  {23170, 60},  {23170, 60},  {23170, 60},
    {23170, 60},  {23170, 60},  {23170, 60},  {23170, 60},  {23170, 61},  {23170, 61},  {23170, 61},  {23170, 61},
    {23170, 61},  {23170, 61},  {23170, 61},  {23170, 62},  {23170, 62},  {23170, 62},  {23170, 62},  {23170, 62},
    {23170, 62},  {23170, 62},  {23170, 62},  {23170, 63},  {23170, 63},  {23170, 63},  {23170, 63},  {23170, 63},
    {23170, 63},  {23170, 63},  {23170, 64},  {23170, 64},  {23170, 64},  {23170, 64},  {23170, 64},  {23170, 64},
    {23170, 64},  {23170, 65},  {23170, 65},  {23170, 65},  {23170, 65},  {23170, 65},  {23170, 65},  {23170, 65},
    {23170, 66},  {23170, 66},  {23170, 66},  {23170, 66},  {23170, 66},  {23170, 66},  {23170, 66},  {23170, 67},
    {23170, 67},  {23170, 67},  {23170, 67},  {23170, 67},  {23170, 67},  {23170, 67},  {23170, 67},  {23170, 68},
    {23170, 68},  {23170, 68},  {23170, 68},  {23170, 68},  {23170, 68},  {23170, 68},  {23170, 69},  {23170, 69},
    {23170, 69},  {23170, 69},  {23170, 69},  {23170, 69},  {23170, 69},  {23170, 70},  {23170, 70},  {23170, 70},
    {23170, 70},  {23170, 70},  {23170, 70},  {23170, 70},  {23170, 71},  {23170, 71},  {23170, 71},  {23170, 71},
    {23170, 71},  {23170, 71},  {23170, 71},  {23170, 72},  {23170, 72},  {23170, 72},  {23170, 72},  {23170, 72},
    {23170, 72},  {23170, 72},  {23170, 72},  {23170, 73},  {23170, 73},  {23170, 73},  {23170, 73},  {23170, 73},
    {23170, 73},  {23170, 73},  {23170, 74},  {23170, 74},  {23170, 74},  {23170, 74},  {23170, 74},  {23170, 74},
    {23170, 74},  {23170, 75},  {23170, 75},  {23170, 75},  {23170, 75},  {23170, 75},  {23170, 75},  {23170, 75},
    {23170, 76},  {23170, 76},  {23170, 76},  {23170, 76},  {23170, 76},  {23170, 76},  {23170, 76},  {23170, 76},
    {23170, 77},  {23170, 77},  {23170, 77},  {23170, 77},  {23170, 77},  {23170, 77},  {23170, 77},  {23170, 78},
    {23170, 78},  {23170, 78},  {23170, 78},  {23170, 78},  {23170, 78},  {23170, 78},  {23170, 79},  {23170, 79},
    {23170, 79},  {23170, 79},  {23170, 79},  {23170, 79},  {23170, 79},  {23170, 80},  {23170, 80},  {23170, 80},
    {23170, 80},  {23170, 80},  {23170, 80},  {23170, 80},  {23170, 81},  {23170, 81},  {23170, 81},  {23170, 81},
    {23170, 81},  {23170, 81},  {23170, 81},  {23170, 81},  {23170, 82},  {23170, 82},  {23170, 82},  {23170, 82},
    {23170, 82},  {23170, 82},  {23170, 82},  {23170, 83},  {23170, 83},  {23170, 83},  {23170, 83},  {23170, 83},
    {23170, 83},  {23170, 83},  {23170, 84},  {23170, 84},  {23170, 84},  {23170, 84},  {23170, 84},  {23170, 84},
    {23170, 84},  {23170, 85},  {23170, 85},  {23170, 85},  {23170, 85},  {23170, 85},  {23170, 85},  {23170, 85},
    {23170, 86},  {23170, 86},  {23170, 86},  {23170, 86},  {23170, 86},  {23170, 86},  {23170, 86},  {23170, 86},
    {23170, 87},  {23170, 87},  {23170, 87},  {23170, 87},  {23170, 87},  {23170, 87},  {23170, 87},  {23170, 88},
    {23170, 88},  {23170, 88},  {23170, 88},  {23170, 88},  {23170, 88},  {23170, 88},  {23170, 89},  {23170, 89},
    {23170, 89},  {23170, 89},  {23170, 89},  {23170, 89},  {23170, 89},  {23170, 90},  {23170, 90},  {23170, 90},
    {23170, 90},  {23170, 90},  {23170, 90},  {23170, 90},  {23170, 91},  {23170, 91},  {23170, 91},  {23170, 91},
    {23170, 91},  {23170, 91},  {23170, 91},  {23170, 91},  {23170, 92},  {23170, 92},  {23170, 92},  {23170, 92},
    {23170, 92},  {23170, 92},  {23170, 92},  {23170, 93},  {23170, 93},  {23170, 93},  {23170, 93},  {23170, 93},
    {23170, 93},  {23170, 93},  {23170, 94},  {23170, 94},  {23170, 94},  {23170, 94},  {23170, 94},  {23170, 94},
    {23170, 94},  {23170, 95},  {23170, 95},  {23170, 95},  {23170, 95},  {23170, 95},  {23170, 95},  {23170, 95},
    {23170, 96},  {23170, 96},  {23170, 96},  {23170, 96},  {23170, 96},  {23170, 96},  {23170, 96},  {23170, 96},
    {23170, 97},  {23170, 97},  {23170, 97},  {23170, 97},  {23170, 97},  {23170, 97},  {23170, 97},  {23170, 98},
    {23170, 98},  {23170, 98},  {23170, 98},  {23170, 98},  {23170, 98},  {23170, 98},  {23170, 99},  {23170, 99},
    {23170, 99},  {23170, 99},  {23170, 99},  {23170, 99},  {23170, 99},  {23170, 100}, {23170, 100}, {23170, 100},
    {23170, 100}, {23170, 100}, {23170, 100}, {23170, 100}, {23170, 101}, {23170, 101}, {23170, 101}, {23170, 101},
    {23170, 101}, {23170, 101}, {23170, 101}, {23170, 101}, {23170, 102}, {23170, 102}, {23170, 102}, {23170, 102},
    {23170, 102}, {23170, 102}, {23170, 102}, {23170, 103}, {23170, 103}, {23170, 103}, {23170, 103}, {23170, 103},
    {23170, 103}, {23170, 103}, {23170, 104}, {23170, 104}, {23170, 104}, {23170, 104}, {23170, 104}, {23170, 104},
    {23170, 104}, {23170, 105}, {23170, 105}, {23170, 105}, {23170, 105}, {23170, 105}, {23170, 105}, {23170, 105},
    {23170, 106}, {23170, 106}, {23170, 106}, {23170, 106}, {23170, 106}, {23170, 106}, {23170, 106}, {23170, 106},
    {23170, 107}, {23170, 107}, {23170, 107}, {23170, 107}, {23170, 107}, {23170, 107}, {23170, 107}, {23170, 108},
    {23170, 108}, {23170, 108}, {23170, 108}, {23170, 108}, {23170, 108}, {23170, 108}, {23170, 109}, {23170, 109},
    {23170, 109}, {23170, 109}, {23170, 109}, {23170, 109}, {23170, 109}, {23170, 110}, {23170, 110}, {23170, 110},
    {23170, 110}, {23170, 110}, {23170, 110}, {23170, 110}, {23170, 111}, {23170, 111}, {23170, 111}, {23170, 111},
    {23170, 111}, {23170, 111}, {23170, 111}, {23169, 111}, {23169, 112}, {23169, 112}, {23169, 112}, {23169, 112},
    {23169, 112}, {23169, 112}, {23169, 112}, {23169, 113}, {23169, 113}, {23169, 113}, {23169, 113}, {23169, 113},
    {23169, 113}, {23169, 113}, {23169, 114}, {23169, 114}, {23169, 114}, {23169, 114}, {23169, 114}, {23169, 114},
    {23169, 114}, {23169, 115}, {23169, 115}, {23169, 115}, {23169, 115}, {23169, 115}, {23169, 115}, {23169, 115},
    {23169, 116}, {23169, 116}, {23169, 116}, {23169, 116}, {23169, 116}, {23169, 116}, {23169, 116}, {23169, 116},
    {23169, 117}, {23169, 117}, {23169, 117}, {23169, 117}, {23169, 117}, {23169, 117}, {23169, 117}, {23169, 118},
    {23169, 118}, {23169, 118}, {23169, 118}, {23169, 118}, {23169, 118}, {23169, 118}, {23169, 119}, {23169, 119},
    {23169, 119}, {23169, 119}, {23169, 119}, {23169, 119}, {23169, 119}, {23169, 120}, {23169, 120}, {23169, 120},
    {23169, 120}, {23169, 120}, {23169, 120}, {23169, 120}, {23169, 121}, {23169, 121}, {23169, 121}, {23169, 121},
    {23169, 121}, {23169, 121}, {23169, 121}, {23169, 121}, {23169, 122}, {23169, 122}, {23169, 122}, {23169, 122},
    {23169, 122}, {23169, 122}, {23169, 122}, {23169, 123}, {23169, 123}, {23169, 123}, {23169, 123}, {23169, 123},
    {23169, 123}, {23169, 123}, {23169, 124}, {23169, 124}, {23169, 124}, {23169, 124}, {23169, 124}, {23169, 124},
    {23169, 124}, {23169, 125}, {23169, 125}, {23169, 125}, {23169, 125}, {23169, 125}, {23169, 125}, {23169, 125},
    {23169, 126}, {23169, 126}, {23169, 126}, {23169, 126}, {23169, 126}, {23169, 126}, {23169, 126}, {23169, 126},
    {23169, 127}, {23169, 127}, {23169, 127}, {23169, 127}, {23169, 127}, {23169, 127}, {23169, 127}, {23169, 128},
    {23169, 128}, {23169, 128}, {23169, 128}, {23169, 128}, {23169, 128}, {23169, 128}, {23169, 129}, {23169, 129},
    {23169, 129}, {23169, 129}, {23169, 129}, {23169, 129}, {23169, 129}, {23169, 130}, {23169, 130}, {23169, 130},
    {23169, 130}, {23169, 130}, {23169, 130}, {23169, 130}, {23169, 131}, {23169, 131}, {23169, 131}, {23169, 131},
    {23169, 131}, {23169, 131}, {23169, 131}, {23169, 131}, {23169, 132}, {23169, 132}, {23169, 132}, {23169, 132},
    {23169, 132}, {23169, 132}, {23169, 132}, {23169, 133}, {23169, 133}, {23169, 133}, {23169, 133}, {23169, 133},
    {23169, 133}, {23169, 133}, {23169, 134}, {23169, 134}, {23169, 134}, {23169, 134}, {23169, 134}, {23169, 134},
    {23169, 134}, {23169, 135}, {23169, 135}, {23169, 135}, {23169, 135}, {23169, 135}, {23169, 135}, {23169, 135},
    {23169, 136}, {23169, 136}, {23169, 136}, {23169, 136}, {23169, 136}, {23169, 136}, {23169, 136}, {23169, 136},
    {23169, 137}, {23169, 137}, {23169, 137}, {23169, 137}, {23169, 137}, {23169, 137}, {23169, 137}, {23169, 138},
    {23169, 138}, {23169, 138}, {23169, 138}, {23169, 138}, {23169, 138}, {23169, 138}, {23169, 139}, {23169, 139},
    {23169, 139}, {23169, 139}, {23169, 139}, {23169, 139}, {23169, 139}, {23169, 140}, {23169, 140}, {23169, 140},
    {23169, 140}, {23169, 140}, {23169, 140}, {23169, 140}, {23169, 141}, {23169, 141}, {23169, 141}, {23169, 141},
    {23169, 141}, {23169, 141}, {23169, 141}, {23169, 141}, {23169, 142}, {23169, 142}, {23169, 142}, {23169, 142}};