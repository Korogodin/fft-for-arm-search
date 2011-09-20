//_________________________________________________________________________________________
//_________________________________________________________________________________________
//
// NAME:          FFT.
// PURPOSE:       Быстрое преобразование Фурье: Комплексный сигнал в комплексный спектр и обратно.
//                В случае действительного сигнала в мнимую часть (Idat) записываются нули.
//                Количество отсчетов - кратно 2**К - т.е. 2, 4, 8, 16, ... (см. комментарии ниже).
//                (C) Sergey Aleynik.  saleynik@yandex.ru
//              
// PARAMETERS:  
//
//    float *Rdat    [in, out] - Real part of Input and Output Data (Signal or Spectrum)
//    float *Idat    [in, out] - Imaginary part of Input and Output Data (Signal or Spectrum)
//    int    N       [in]      - Input and Output Data lendth (Number of samples in arrays)
//    int    LogN    [in]      - Logarithm2(N)
//    int    Ft_Flag [in]      - Ft_Flag = FT_ERR_DIRECT  (i.e. -1) - Direct  FFT  (Signal to Spectrum)
//                             Ft_Flag = FT_ERR_INVERSE (i.e.  1) - Inverse FFT  (Spectrum to Signal)
//
// RETURN VALUE:  false on parameter error, true on success.
//_________________________________________________________________________________________
//
// NOTE: In this algorithm N and LogN can be only:
//       N    = 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384;
//       LogN = 2, 3,  4,  5,  6,   7,   8,   9,   10,   11,   12,   13,    14;
//_________________________________________________________________________________________
//_________________________________________________________________________________________
 
 
#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.
 

#include <stddef.h>
#include <stdio.h>
// typedef int qint16;

bool  FFT(qint16 *Rdat, qint16 *Idat, qint16 N, qint16 LogN, qint16 Ft_Flag)
{
  // parameters error check:
  if((Rdat == NULL) || (Idat == NULL))                  return false;
  if((N > 16384) || (N < 1))                            return false;
  if(!NUMBER_IS_2_POW_K(N))                             return false;
  if((LogN < 2) || (LogN > 14))                         return false;
  if((Ft_Flag != FT_DIRECT) && (Ft_Flag != FT_INVERSE)) return false;
 
  register qint16  i, j, n, k, io, ie, in, nn;
  qint16         ru, iu, rtp, itp, rtq, itq, rw, iw, sr;
 
  static const qint16 Rcoef[14] =
  {  -4096, 0, 2896, 3784, 4017, 
      4076, 4091, 4095, 4096, 4096, 
      4096, 4096, 4096, 4096
  };
  static const qint16 Icoef[14] =
  {   0, -4096, -2896, -1567, -799, 
      -401, -201, -101, -50, -25, 
      -13, -6, -3, -2 
  };
 
  nn = N >> 1;
  ie = N;
  for(n=1; n<=LogN; n++)
  {
    rw = Rcoef[LogN - n];
    iw = Icoef[LogN - n];
    if(Ft_Flag == FT_INVERSE) iw = -iw;
    in = ie >> 1;
    ru = 1;
    iu = 0;
    for(j=0; j<in; j++)
    {
      for(i=j; i<N; i+=ie)
      {
        io       = i + in;
        rtp      = Rdat[i]  + Rdat[io];
        itp      = Idat[i]  + Idat[io];
        rtq      = Rdat[i]  - Rdat[io];
        itq      = Idat[i]  - Idat[io];
        Rdat[io] = rtq * ru - itq * iu;
        Idat[io] = itq * ru + rtq * iu;
        Rdat[i]  = rtp;
        Idat[i]  = itp;
      }
 
      sr = ru;
      ru = ru * rw - iu * iw;
      iu = iu * rw + sr * iw;
    }
 
    ie >>= 1;
  }
 
  for(j=i=1; i<N; i++)
  {
    if(i < j)
    {
      io       = i - 1;
      in       = j - 1;
      rtp      = Rdat[in];
      itp      = Idat[in];
      Rdat[in] = Rdat[io];
      Idat[in] = Idat[io];
      Rdat[io] = rtp;
      Idat[io] = itp;
    }
 
    k = nn;
 
    while(k < j)
    {
      j   = j - k;
      k >>= 1;
    }
 
    j = j + k;
  }
 
  if(Ft_Flag == FT_INVERSE) return true;
 
   
  for(i=0; i<N; i++)
  {
    Rdat[i] /=  N;
    Idat[i] /=  N;
  }
 
  return true;
}



void FFT_probe(){
  
  qint16 RealData[2048] = {
    52410, 13665, 58737, 48871, 35153, 63568, 37020, 14267, 56214, 56492, 20533, 21429, 55051, 32279, 3401, 50988, 
    27962, 18348, 21857, 24048, 52088, 2536, 47623, 57192, 18731, 43047, 15197, 40760, 4923, 63357, 39975, 25147, 
    2003, 56201, 39554, 55564, 33071, 518, 60232, 26916, 47996, 9903, 53427, 59612, 50983, 49619, 26555, 46000, 
    37058, 38321, 22604, 46153, 10542, 99, 36831, 35814, 36099, 45517, 60784, 61880, 59196, 39591, 60700, 7664, 
    22335, 2407, 35283, 48315, 11877, 27935, 6434, 19872, 51126, 35190, 50402, 41874, 58533, 3977, 11519, 27285, 
    48483, 58521, 1694, 9015, 27793, 50107, 34370, 49446, 11128, 44088, 40544, 449, 48511, 64994, 8400, 23631, 
    12522, 46819, 11657, 64674, 1114, 2608, 52713, 56347, 37110, 49422, 34047, 38398, 12500, 32994, 3337, 3676, 
    21967, 41347, 58457, 44132, 44910, 45596, 52418, 43293, 34075, 21741, 61222, 16153, 33502, 48517, 16657, 55416, 
    35268, 59647, 22984, 61297, 60799, 38516, 6669, 24044, 18078, 17439, 14846, 87, 58524, 29720, 37907, 20684, 
    65144, 64480, 63213, 43664, 47590, 21892, 34260, 17915, 47080, 50988, 5313, 14521, 13367, 40901, 47527, 54681, 
    1238, 13246, 30742, 24799, 22308, 4185, 49895, 26389, 44190, 36115, 3372, 20152, 63271, 61045, 24774, 40480, 
    36866, 54432, 62764, 4948, 58198, 36861, 12737, 14513, 46253, 39109, 38423, 63471, 38125, 6541, 10918, 6691, 
    9584, 44012, 41933, 24408, 10674, 25528, 52427, 26201, 49487, 19347, 41943, 58007, 13739, 31481, 7405, 8675, 
    4194, 5197, 41088, 26957, 41851, 56135, 50042, 63974, 51219, 61314, 48441, 16621, 47137, 45450, 50680, 48970, 
    20866, 33452, 50664, 37540, 62512, 11244, 59457, 49297, 18755, 41118, 30332, 8483, 36004, 63538, 28976, 38729, 
    7273, 13098, 10680, 2417, 17873, 15082, 23498, 8804, 65444, 33654, 25418, 16374, 23904, 26153, 60706, 32471, 
    39924, 315, 38517, 50952, 43069, 34624, 54118, 63074, 20561, 52238, 18689, 976, 6170, 21543, 20026, 1181, 
    10630, 29097, 50251, 44647, 46828, 30132, 60245, 64811, 61119, 30246, 59305, 25326, 39519, 36721, 55433, 18665, 
    43471, 39470, 43022, 20312, 21735, 12334, 6598, 18780, 23254, 35112, 64935, 1847, 46500, 59320, 56743, 7810, 
    62605, 28895, 57394, 56685, 23275, 41363, 56659, 1376, 5034, 24688, 9778, 2235, 51266, 21446, 53574, 11378, 
    44310, 57383, 49621, 15049, 23524, 23800, 17527, 22105, 5699, 29593, 29797, 1903, 41753, 3897, 11088, 44871, 
    36344, 396, 18880, 24682, 9601, 4885, 30195, 24274, 54038, 35228, 53577, 30172, 920, 327, 10894, 23982, 
    47069, 10448, 18270, 42267, 18822, 21098, 10185, 25406, 58703, 58181, 25796, 44267, 16529, 62258, 40939, 13563, 
    7186, 37084, 17979, 4664, 10384, 3245, 44622, 51233, 52911, 17371, 58714, 39845, 944, 22546, 34974, 41141, 
    29277, 53157, 9491, 63886, 54641, 22289, 40386, 19907, 5847, 34179, 54086, 50065, 62104, 21859, 25542, 9857, 
    21870, 36286, 36064, 10513, 7674, 26119, 54486, 12124, 32819, 8278, 56664, 50243, 36985, 25531, 32122, 58124, 
    59313, 32662, 34683, 59618, 37919, 50842, 43345, 30779, 14403, 39491, 12076, 12944, 56492, 8235, 42309, 28660, 
    40148, 48332, 19876, 2816, 54756, 24203, 43342, 58702, 17963, 65398, 54680, 51861, 43032, 35630, 25342, 53886, 
    39016, 51226, 64728, 51659, 55897, 31825, 57262, 21879, 13456, 32398, 2045, 54236, 17318, 44401, 52027, 44685, 
    42648, 15554, 31287, 61369, 15801, 13704, 17857, 50844, 21719, 39523, 12060, 5732, 20245, 15131, 59584, 61400, 
    2089, 38905, 2872, 27846, 34182, 55072, 40962, 16725, 59290, 50286, 36875, 58796, 24122, 22134, 40557, 5695, 
    24609, 12280, 53119, 47399, 44215, 59899, 53802, 35961, 1019, 6205, 33735, 62422, 55251, 61277, 63794, 24414, 
    21031, 51331, 13908, 62859, 60964, 23968, 22604, 45396, 46963, 52451, 38907, 6368, 36668, 2824, 49652, 43231, 
    39286, 14876, 46774, 47481, 22559, 2146, 55287, 50482, 33400, 46332, 51850, 21671, 59879, 37795, 18868, 17489, 
    58339, 23993, 9831, 62019, 42653, 64370, 12703, 54312, 32362, 38717, 2942, 36992, 41520, 32643, 3907, 33762, 
    3486, 55288, 38972, 45287, 42417, 40858, 62512, 11596, 31123, 55208, 32569, 49965, 11788, 45326, 59928, 10660, 
    28056, 2743, 14795, 43657, 57104, 22592, 1786, 59329, 30872, 59668, 21494, 45292, 1143, 33577, 21746, 22027, 
    62478, 49185, 21884, 41473, 27233, 37781, 62882, 40360, 9868, 54937, 47590, 7133, 10574, 19465, 27760, 34630, 
    18029, 54261, 51956, 17940, 40350, 6655, 43907, 22767, 43876, 19353, 44593, 25806, 25678, 13564, 47482, 53694, 
    60717, 33693, 12658, 39512, 12988, 26895, 55907, 28120, 60664, 24450, 45760, 38576, 6610, 27027, 60602, 3641, 
    25169, 15771, 14930, 23404, 60562, 32932, 52641, 22955, 30060, 62331, 8715, 4608, 46274, 61060, 7137, 42710, 
    9572, 45480, 41000, 2865, 52293, 64632, 28623, 22593, 3199, 14759, 38077, 39412, 16549, 42883, 3938, 61637, 
    6064, 21888, 42667, 33024, 34795, 24846, 50391, 18515, 42551, 60436, 21671, 53310, 51618, 8148, 10315, 27197, 
    45045, 43103, 44563, 615, 8429, 50063, 55176, 48952, 61900, 60274, 14619, 33408, 51625, 34239, 40461, 36585, 
    47906, 4155, 32295, 15068, 22111, 36152, 48738, 27021, 33267, 8452, 14079, 19191, 29489, 15747, 17253, 58431, 
    38413, 61517, 40506, 16431, 6720, 35807, 29009, 45319, 49771, 26176, 37186, 13473, 61835, 33607, 18104, 15017, 
    62451, 45182, 46391, 43052, 51050, 38042, 32980, 57589, 25752, 15901, 18457, 59295, 46051, 55906, 24817, 41486, 
    2068, 60295, 30785, 34072, 50221, 22034, 41025, 21424, 20389, 42341, 28422, 6743, 15476, 63687, 45803, 43218, 
    21371, 1299, 37386, 2381, 17350, 12495, 29445, 50375, 41152, 20624, 19139, 5127, 11269, 47270, 5862, 60150, 
    28159, 5225, 37496, 54511, 22765, 51111, 46422, 38728, 24707, 25810, 28070, 27723, 14347, 32556, 2613, 37016, 
    3221, 57166, 56363, 22168, 63834, 62572, 64790, 10336, 191, 16265, 22694, 15292, 49538, 14806, 29389, 17003, 
    22662, 59176, 832, 8665, 25432, 55091, 13977, 61292, 63557, 24593, 64544, 8001, 49870, 37969, 40708, 32106, 
    14447, 9446, 41627, 52587, 43886, 30907, 61897, 38763, 14860, 44816, 21014, 49081, 65414, 17677, 61071, 45638, 
    6243, 16113, 61254, 31443, 57837, 51072, 1198, 50513, 44944, 56863, 38044, 41032, 7284, 14005, 2384, 29134, 
    21403, 18828, 32535, 11908, 61197, 61616, 38762, 53, 59179, 44754, 4840, 65299, 20608, 53033, 30242, 38479, 
    31300, 36206, 11490, 38486, 10145, 29193, 35234, 30271, 54664, 65285, 63717, 48885, 13104, 2381, 12360, 37850, 
    41174, 23906, 41717, 58412, 16615, 18767, 8219, 34641, 53790, 19646, 51621, 28640, 36080, 64626, 45205, 63579, 
    6289, 58857, 3830, 8082, 41335, 24255, 20943, 52147, 19773, 39811, 30276, 19175, 54553, 37390, 4621, 17453, 
    11198, 36720, 13244, 32268, 47800, 62587, 49963, 5247, 61489, 63863, 39788, 62750, 21670, 59062, 24, 31672, 
    9665, 20643, 11231, 38857, 43097, 22870, 3519, 3091, 3661, 4687, 1027, 63811, 48156, 49308, 2183, 7707, 
    17125, 58547, 63705, 65148, 22727, 19156, 12271, 48703, 47056, 12379, 28439, 2988, 42957, 51186, 56551, 51469, 
    28409, 22795, 3630, 53703, 6949, 398, 18025, 61785, 44188, 25275, 56161, 44225, 39852, 28798, 62999, 53930, 
    688, 9281, 31641, 38051, 19201, 28762, 26101, 8998, 62440, 23596, 54852, 2709, 27776, 4957, 20566, 61996, 
    56001, 11373, 35120, 19326, 59171, 5334, 51608, 11430, 47185, 15205, 13575, 14164, 64007, 25025, 41634, 3052, 
    15209, 57334, 30156, 201, 30470, 54006, 57140, 19731, 15467, 54498, 28689, 33232, 12155, 9929, 59428, 65061, 
    25564, 54186, 62525, 58579, 55116, 44780, 11259, 15568, 28461, 1642, 17606, 64236, 20493, 37470, 40086, 59083, 
    25955, 45401, 13401, 4119, 40789, 45090, 19450, 37458, 20724, 53051, 23953, 45796, 41512, 63215, 13159, 39202, 
    5486, 35679, 55453, 2667, 21585, 46488, 56381, 32827, 65193, 28091, 17521, 30995, 10541, 10414, 17586, 49619, 
    39923, 63369, 50680, 37543, 29506, 18813, 49308, 6184, 6998, 48162, 23239, 38052, 19586, 46772, 23625, 47085, 
    65230, 5485, 30684, 6147, 47525, 59772, 5663, 34076, 36217, 44902, 19638, 39445, 13009, 43041, 45768, 30126, 
    10332, 27642, 39063, 21124, 54443, 8040, 16461, 61447, 42940, 49343, 53140, 3174, 27178, 47546, 9112, 41455, 
    16176, 55048, 54832, 29693, 25871, 63019, 536, 26112, 31967, 40510, 4588, 9351, 42875, 14308, 7807, 2827, 
    10850, 33496, 56837, 50218, 8844, 4255, 44069, 31602, 32468, 20544, 21201, 59579, 29513, 49063, 12141, 5565, 
    13568, 36629, 43365, 61552, 2638, 10726, 28713, 21343, 5894, 53105, 26185, 42947, 37149, 205, 42687, 20781, 
    17428, 30176, 40839, 52550, 36776, 1626, 52005, 17897, 53930, 45125, 39577, 25359, 4295, 65444, 43322, 15019, 
    6046, 15793, 59151, 14244, 31426, 59587, 12700, 29359, 40488, 38942, 43639, 17543, 27779, 52384, 61100, 30784, 
    20074, 28795, 20110, 18023, 9093, 10915, 32371, 5674, 60741, 9731, 10517, 47037, 44230, 23496, 5928, 541, 
    24430, 51662, 29313, 6529, 52811, 43218, 29646, 34954, 34516, 41267, 42373, 12572, 52542, 12882, 26955, 15446, 
    17839, 568, 60836, 34093, 30417, 34310, 22073, 21783, 64768, 31046, 48946, 43906, 52140, 17861, 18607, 47006, 
    12100, 54638, 11119, 62473, 47353, 8798, 55908, 61059, 36974, 26137, 28941, 45985, 17042, 1870, 62126, 65285, 
    51084, 64841, 31952, 8387, 30433, 48666, 44131, 57277, 49693, 40447, 18010, 22045, 54770, 25289, 5266, 56727, 
    28533, 11277, 282, 42762, 24228, 24648, 28924, 33825, 19668, 31347, 409, 11235, 19894, 48789, 38069, 45954, 
    52734, 55836, 7715, 43074, 49345, 46015, 39657, 2508, 12789, 21672, 42129, 14248, 5040, 64264, 13550, 64438, 
    40001, 16252, 33945, 46716, 17031, 7516, 36023, 14346, 20459, 63775, 20263, 62013, 26907, 677, 20321, 10172, 
    65148, 29507, 35731, 17192, 57838, 6754, 53973, 27995, 60316, 34604, 57071, 23122, 26224, 39185, 59737, 8715, 
    2763, 26282, 39040, 60272, 19253, 35720, 51228, 21456, 6243, 28946, 1906, 44262, 7313, 13084, 34745, 41114, 
    24484, 39745, 55469, 46003, 25747, 37478, 16369, 47667, 22500, 5153, 19807, 19687, 1522, 11542, 56421, 42988, 
    56134, 17756, 58717, 13683, 63188, 27156, 15045, 53633, 23622, 32868, 49000, 54516, 25852, 6333, 17393, 51901, 
    61398, 37241, 61474, 39141, 64746, 49961, 12165, 33867, 41953, 56948, 39329, 65514, 8466, 19226, 31253, 8492, 
    14950, 11867, 44041, 21351, 51812, 35784, 15110, 45008, 3217, 37526, 65127, 23788, 36616, 60770, 17031, 54557, 
    11477, 57661, 18326, 55671, 52869, 12395, 64131, 16628, 51023, 53136, 32634, 60013, 6618, 8901, 56251, 1222, 
    47732, 53520, 7617, 32018, 22770, 50718, 18697, 54028, 11342, 56600, 60690, 12524, 3300, 35943, 624, 54288, 
    59542, 46222, 15947, 38616, 61532, 58688, 39730, 51929, 54799, 36709, 46, 16468, 28308, 28223, 57359, 4433, 
    22276, 11604, 16124, 47698, 18888, 15554, 9153, 14466, 53579, 24788, 43615, 61329, 27950, 39660, 63543, 23898, 
    43545, 20628, 58046, 19369, 19357, 59447, 32302, 13575, 1692, 11155, 19814, 63009, 51358, 44123, 55098, 27581, 
    46120, 31542, 53777, 28665, 56359, 583, 65276, 18036, 2197, 9968, 31431, 58251, 18574, 51389, 11167, 47926, 
    16163, 5364, 55896, 44994, 62791, 64557, 35044, 59374, 31238, 58305, 1563, 8436, 48539, 25626, 25797, 39443, 
    1565, 25406, 3719, 45192, 34104, 4986, 26625, 28194, 43209, 14312, 5314, 30649, 47754, 40238, 59022, 16566, 
    14424, 15454, 53093, 46033, 57574, 46597, 43325, 28808, 36883, 64901, 62739, 22074, 10188, 27334, 36338, 1950, 
    56800, 58616, 32584, 13716, 47291, 45857, 30196, 11180, 52060, 58229, 9340, 61582, 17, 28758, 732, 15369, 
    46333, 45493, 17375, 21402, 6110, 2726, 49760, 21291, 34198, 62383, 43842, 31006, 42557, 43623, 15083, 52749, 
    32951, 28445, 46467, 36390, 16712, 4928, 38607, 44187, 38536, 34977, 30973, 21221, 28245, 50128, 65304, 17368, 
    51909, 8822, 2329, 29750, 36895, 15131, 9487, 44374, 23204, 33454, 45528, 39013, 33168, 50882, 44905, 20938, 
    54535, 43308, 38474, 1250, 54850, 7471, 55078, 22118, 28143, 52021, 9278, 39853, 60990, 41710, 3131, 45767, 
    16850, 45279, 23801, 35513, 14558, 59342, 24480, 5165, 2027, 41085, 61407, 20967, 48836, 18935, 8150, 33691, 
    25396, 64134, 19125, 38820, 37444, 40361, 36760, 18341, 9952, 48039, 2602, 52535, 26635, 54396, 63638, 27688, 
    3533, 51579, 41293, 33346, 37204, 20263, 48964, 22063, 57892, 21866, 52168, 52742, 3292, 26379, 63255, 27157, 
    21661, 41705, 56232, 51661, 35285, 48606, 48478, 29150, 55732, 32410, 62138, 52284, 8719, 15582, 8370, 35004, 
    3716, 30709, 64401, 44710, 57075, 32520, 18852, 11994, 62994, 49562, 62282, 43992, 12193, 18995, 3815, 2888, 
    18321, 11172, 44374, 59141, 23068, 6564, 51970, 30699, 45046, 44766, 27619, 43211, 41512, 34416, 12536, 33548, 
    49043, 27514, 48331, 44988, 62634, 20179, 22433, 25365, 1715, 41520, 37044, 23954, 27621, 30291, 20607, 14789, 
    7598, 8780, 62031, 32272, 49035, 20377, 505, 40345, 23434, 42522, 55442, 6642, 38555, 38167, 34197, 29793, 
    63354, 11948, 36660, 41057, 1975, 42092, 54930, 25896, 28662, 5563, 38101, 57127, 13560, 6200, 12318, 43767, 
    48000, 41354, 30684, 20602, 42039, 36833, 43498, 8184, 44173, 44585, 60956, 54318, 11672, 62209, 13082, 14266, 
    558, 19702, 17988, 38433, 25988, 12216, 25744, 10131, 54546, 9814, 44535, 49158, 976, 9322, 2009, 52999, 
    3090, 43659, 29887, 52625, 23491, 1660, 14325, 46591, 35974, 56996, 48420, 6765, 63664, 48412, 908, 44507, 
    31818, 2275, 7500, 5022, 39834, 15300, 9307, 34745, 60968, 40429, 45213, 15363, 13556, 2449, 43474, 33243, 
    31837, 56472, 63276, 49787, 16166, 44320, 49762, 38417, 53899, 60741, 41784, 1241, 12081, 1752, 61276, 31750, 
    52087, 29939, 32388, 27900, 58443, 57297, 41494, 19155, 19115, 16089, 46160, 50765, 53913, 52834, 16631, 52462, 
    41269, 15172, 24974, 29701, 32805, 35427, 22619, 19099, 46106, 61216, 38940, 38869, 48413, 24422, 29706, 33382, 
    60864, 29543, 41775, 30105, 36292, 2702, 52573, 44008, 13129, 956, 50015, 37192, 42875, 41598, 48125, 58152, 
    43641, 7909, 46775, 29309, 42007, 42526, 44458, 3855, 14677, 11009, 60852, 33766, 44130, 43765, 5339, 14750, 
    18625, 59884, 15014, 16294, 44321, 42499, 46116, 54860, 47664, 1274, 38256, 60351, 6008, 36344, 61001, 13071, 
    34427, 8306, 49069, 20622, 12267, 64251, 46716, 54110, 44775, 45181, 34791, 12377, 32310, 36670, 54282, 13295, 
    34586, 52941, 23272, 35898, 47018, 13272, 42189, 6, 33632, 37344, 461, 58489, 54604, 50801, 51651, 24486, 
    9982, 23086, 42277, 61075, 6116, 48417, 3622, 49621, 30375, 2949, 55183, 10795, 7541, 17797, 20604, 39721, 
    44398, 64726, 65094, 49646, 18026, 62522, 26941, 14197, 41231, 974, 2838, 11814, 13138, 47144, 29024, 55410, 
    25545, 54890, 49048, 38264, 10523, 34658, 30322, 24874, 6110, 16983, 22009, 24583, 9585, 20760, 18430, 53748 };

  qint16 ImagData[2048] = {
    22419, 57148, 17566, 50819, 39925, 1178, 45974, 1032, 44820, 57534, 28299, 41376, 38407, 14904, 51423, 18851, 
    60580, 19561, 35147, 21866, 15551, 35733, 7057, 8989, 6188, 20374, 53257, 33657, 56785, 64257, 21408, 14393, 
    33962, 23135, 56327, 17041, 55294, 38194, 47147, 22513, 608, 12612, 33252, 1589, 36048, 18100, 45629, 57760, 
    1618, 22361, 27617, 5244, 5201, 4743, 59005, 63525, 25645, 20551, 36262, 51908, 52318, 56575, 52295, 418, 
    60287, 1177, 1928, 46626, 35286, 34379, 32915, 4304, 15076, 7670, 64866, 4621, 3934, 43357, 22548, 8422, 
    41755, 48922, 52774, 17631, 28441, 26333, 39855, 50468, 61678, 8616, 16759, 24793, 65125, 22345, 58953, 15564, 
    14418, 64951, 62332, 41826, 33039, 23470, 50361, 51404, 1895, 3437, 21196, 52408, 41303, 64409, 10371, 40414, 
    20232, 5576, 57464, 38712, 19865, 11015, 22274, 4420, 42827, 16009, 49671, 19596, 27447, 3659, 2022, 12228, 
    17754, 4341, 21856, 3271, 6639, 4710, 59278, 20434, 38782, 24748, 62362, 48654, 58137, 39249, 45196, 25632, 
    14632, 8602, 11304, 61512, 2260, 57643, 34231, 52969, 29333, 22711, 49621, 23607, 36478, 58243, 55061, 15993, 
    28549, 54300, 10008, 49673, 57443, 32907, 37373, 55803, 64530, 51787, 22982, 32893, 16726, 33415, 5152, 3663, 
    41615, 40429, 5339, 39946, 35696, 56094, 42119, 17802, 6996, 3966, 40756, 45814, 18018, 27654, 23975, 5384, 
    4572, 7923, 44424, 14348, 15550, 566, 47652, 36768, 36174, 48644, 54043, 25782, 8909, 12828, 47206, 43885, 
    3922, 25184, 32194, 50735, 9687, 31820, 4708, 51606, 25804, 15477, 5280, 5178, 60780, 13416, 62690, 22265, 
    62912, 22488, 4156, 40139, 36413, 59405, 13188, 51622, 22788, 63711, 54792, 46649, 14084, 1012, 13765, 35158, 
    7108, 55952, 59694, 21871, 30746, 190, 20376, 31800, 17181, 44768, 59715, 37388, 17385, 60848, 4210, 41045, 
    62146, 33842, 62699, 17827, 34537, 59716, 14070, 54184, 9065, 28688, 57363, 46158, 32573, 39951, 5101, 15823, 
    1690, 55475, 974, 36072, 7843, 38532, 40777, 48957, 20311, 60064, 51267, 6622, 18422, 59474, 7280, 51762, 
    6703, 50328, 43388, 50666, 61078, 11592, 1338, 54633, 31011, 7120, 5836, 7545, 11008, 63734, 18744, 39258, 
    55850, 16042, 47656, 26266, 20356, 42847, 25679, 7821, 64129, 33964, 33277, 21301, 12928, 56551, 17555, 35303, 
    36141, 63128, 17667, 17414, 20315, 36523, 24785, 16106, 44314, 17124, 63012, 32609, 28034, 37211, 40176, 52887, 
    57723, 10484, 5844, 20522, 39803, 24821, 60656, 3736, 65144, 43710, 40446, 9692, 1900, 37517, 1965, 46631, 
    14027, 16724, 62822, 11920, 52328, 2624, 20261, 63379, 17497, 7987, 39805, 42294, 57306, 58807, 3485, 49775, 
    56289, 31570, 4922, 5946, 13121, 51006, 26691, 54201, 9697, 48032, 9823, 54947, 35741, 62991, 4948, 61200, 
    23555, 4386, 63303, 42563, 27566, 39088, 22747, 14262, 2480, 10055, 24470, 57463, 28911, 41797, 25415, 54866, 
    50221, 8230, 6073, 43275, 24751, 7064, 40882, 40737, 59744, 39723, 31808, 37797, 64363, 33985, 22853, 64414, 
    65012, 58731, 48600, 43564, 48062, 28568, 26190, 14280, 57779, 61739, 62191, 6976, 3799, 10730, 34569, 4571, 
    665, 6008, 39809, 3676, 43129, 37737, 4839, 15831, 23016, 8306, 48458, 29208, 1154, 44676, 57726, 9163, 
    53140, 16571, 1598, 34782, 61267, 50419, 26526, 47042, 31544, 28913, 61759, 37931, 51355, 46846, 9317, 45411, 
    25798, 8373, 42002, 32605, 4193, 34311, 20614, 56026, 591, 13277, 57711, 14290, 20820, 22542, 52434, 37427, 
    53296, 5648, 12592, 41255, 4185, 48691, 39544, 52327, 10525, 48361, 49613, 57758, 28131, 12287, 19859, 21021, 
    28967, 23716, 40999, 61527, 18803, 16030, 60349, 60701, 20312, 5012, 51176, 8569, 30244, 28847, 39649, 8324, 
    39259, 8659, 7559, 28906, 17767, 61873, 33859, 29183, 20224, 30776, 51082, 11063, 1471, 46447, 39659, 27111, 
    59950, 31273, 60046, 32565, 60788, 46114, 45524, 301, 35404, 27064, 41165, 35621, 14156, 36183, 56541, 324, 
    8198, 50795, 1336, 9348, 20998, 59294, 40249, 1042, 20408, 12634, 18053, 1750, 54437, 50088, 47262, 53918, 
    50105, 7901, 25188, 1830, 85, 459, 10996, 9859, 33783, 27452, 38916, 18152, 38852, 38348, 7969, 5151, 
    2101, 11021, 34352, 55140, 20081, 12528, 4268, 61998, 16041, 40986, 13545, 15843, 48720, 10224, 53947, 6159, 
    64212, 42809, 18620, 56835, 1975, 49869, 58732, 40559, 47235, 30399, 49734, 468, 12680, 5993, 11006, 61751, 
    55668, 8997, 43301, 2522, 9710, 12109, 42294, 56135, 55927, 32377, 51176, 18000, 63398, 64915, 5534, 21613, 
    17583, 14151, 50268, 23924, 10094, 10815, 58548, 61983, 33913, 1204, 27480, 8001, 62380, 17707, 58566, 58667, 
    50825, 59560, 1093, 62868, 4932, 18687, 31989, 63607, 37232, 17955, 8460, 10171, 17617, 28435, 25946, 27869, 
    40265, 29282, 788, 2246, 26134, 64189, 35571, 59794, 21244, 19039, 57604, 51062, 25848, 57824, 27897, 30565, 
    27848, 29833, 45575, 6264, 48068, 8521, 32541, 17017, 38775, 14917, 62595, 4129, 12009, 40910, 19098, 8991, 
    12382, 62693, 19439, 15545, 43264, 282, 5123, 20289, 14678, 45566, 45890, 55825, 36639, 37377, 34794, 56288, 
    29252, 25125, 39017, 19991, 6626, 24375, 14559, 42155, 50899, 28183, 40702, 18289, 31329, 20627, 54209, 48704, 
    9458, 4474, 34874, 48756, 54771, 47229, 58171, 10673, 13188, 57800, 30275, 12258, 47506, 57905, 52298, 55245, 
    35261, 13736, 34763, 55267, 14113, 46971, 56536, 30885, 30180, 61770, 41179, 52847, 26315, 21063, 24825, 47118, 
    12653, 32112, 7915, 9170, 62075, 41350, 28748, 46384, 34711, 42757, 50999, 40502, 32905, 31809, 7219, 60067, 
    28352, 27984, 1919, 33451, 63157, 18045, 48720, 7655, 50419, 50182, 35440, 11096, 60956, 13645, 1295, 39427, 
    39805, 30727, 39080, 47413, 26746, 39165, 27899, 7985, 22935, 35832, 8330, 5341, 18970, 44356, 29953, 32717, 
    3832, 4824, 22143, 22025, 64550, 49261, 42371, 57859, 33547, 24655, 57918, 16106, 10748, 43162, 16752, 10329, 
    18128, 19576, 387, 14985, 43917, 64334, 16787, 40578, 63690, 32255, 19728, 39131, 35611, 8505, 36190, 9769, 
    16944, 34650, 53305, 47090, 32822, 58588, 62306, 30301, 18848, 60710, 32527, 57497, 13975, 7497, 8147, 37624, 
    30564, 24513, 49454, 29858, 46270, 60916, 53770, 2611, 63320, 58194, 41863, 718, 64046, 25266, 26192, 33442, 
    36152, 62842, 56060, 31241, 57529, 25551, 3222, 20148, 20985, 30231, 31990, 46317, 24071, 61698, 17576, 48850, 
    59577, 38309, 59409, 3715, 36486, 194, 10037, 26705, 15299, 35082, 20421, 46441, 38474, 15676, 4198, 221, 
    12391, 32192, 40401, 8046, 8718, 20590, 5758, 26791, 18502, 41326, 14402, 42877, 12679, 6200, 60186, 36585, 
    27371, 7238, 36225, 7293, 15468, 9087, 21512, 22704, 60867, 58217, 51776, 4880, 27512, 26000, 27062, 63829, 
    15591, 7116, 52431, 2937, 62912, 52537, 62954, 50184, 39861, 64818, 36868, 32499, 16583, 12843, 32292, 46589, 
    64725, 21930, 14429, 22628, 6004, 41393, 24785, 58154, 27953, 58378, 14695, 52228, 56248, 27990, 38592, 6010, 
    16422, 38913, 34117, 59903, 43470, 32745, 41602, 5660, 2674, 49254, 21218, 4358, 60507, 3563, 4453, 37587, 
    28783, 1794, 3257, 15634, 8343, 41309, 31222, 7209, 28536, 59106, 37082, 47883, 64288, 38549, 32975, 34839, 
    39734, 50090, 39362, 47355, 28522, 42371, 11580, 57441, 16495, 23363, 19880, 63423, 12602, 6414, 45468, 6293, 
    13067, 39784, 46512, 58362, 39495, 56522, 44300, 51446, 35170, 291, 13016, 28752, 36588, 25240, 42393, 38383, 
    50222, 11445, 32972, 25079, 64042, 61347, 58506, 29067, 49252, 59599, 24466, 7621, 34414, 22356, 55666, 12205, 
    13680, 34762, 5524, 15779, 41586, 7279, 39803, 58311, 32158, 20546, 49948, 62556, 42329, 31806, 28717, 12215, 
    29041, 31989, 56435, 46954, 40654, 5883, 7324, 37722, 30255, 35747, 2491, 37964, 41221, 31584, 10520, 49439, 
    52003, 19095, 33267, 15597, 12628, 52999, 4370, 12836, 63883, 37689, 38221, 62261, 6556, 57589, 58422, 14027, 
    10941, 42344, 11819, 7638, 23943, 26370, 5886, 44811, 35841, 64616, 13943, 60227, 43592, 48725, 24803, 31114, 
    6964, 57920, 29170, 43793, 47724, 18720, 38908, 24283, 52143, 4819, 65060, 50858, 483, 58258, 56631, 45718, 
    57999, 30516, 24350, 5441, 65354, 57005, 5, 44563, 42918, 26989, 31219, 25446, 29626, 1754, 27872, 6934, 
    2825, 36446, 40061, 48420, 16837, 47596, 46744, 33015, 58509, 51078, 32431, 49906, 43715, 64102, 7672, 56972, 
    37215, 7874, 63592, 55048, 4084, 18224, 26846, 41079, 60234, 27207, 46629, 63627, 28539, 41640, 59084, 57120, 
    9353, 13146, 40693, 50792, 16175, 24212, 22354, 29910, 11195, 48864, 6421, 58345, 5380, 51218, 55619, 29549, 
    39671, 23231, 49748, 38630, 40080, 55258, 29949, 57855, 32347, 40819, 36842, 47295, 35833, 51409, 29252, 59763, 
    5675, 65466, 47683, 36596, 9360, 54895, 47530, 27353, 17903, 20625, 38122, 45178, 56614, 13819, 33210, 27856, 
    15709, 49645, 51899, 25289, 20844, 31518, 18252, 10357, 25913, 18708, 15929, 42108, 62713, 12212, 59606, 37273, 
    12682, 46153, 42702, 21751, 34182, 1623, 1460, 52332, 7066, 45142, 50792, 51871, 46496, 62032, 19531, 65262, 
    36336, 42997, 61954, 14722, 64325, 11182, 61673, 62715, 11470, 3559, 17577, 35783, 50803, 39568, 20716, 28400, 
    55232, 41034, 17862, 33721, 58222, 11423, 53744, 11875, 6374, 2052, 667, 63403, 58198, 3256, 43923, 64004, 
    14435, 5509, 23519, 48050, 39462, 29581, 59821, 208, 33429, 9467, 33073, 37239, 42085, 13562, 22295, 21461, 
    40302, 38744, 45084, 10908, 42744, 50373, 25937, 25970, 45359, 15576, 62186, 8897, 33523, 2715, 20474, 21088, 
    62149, 13588, 32070, 29142, 53795, 1146, 51676, 35441, 2980, 33676, 29204, 32202, 43247, 33120, 48619, 61856, 
    52385, 28687, 5919, 61182, 24797, 49737, 52677, 52706, 39046, 21298, 49044, 31226, 33790, 31463, 27788, 11535, 
    18387, 57519, 57269, 32086, 39733, 47034, 9177, 38211, 12780, 4151, 4188, 61595, 34091, 9838, 42375, 38259, 
    20469, 44050, 26021, 14386, 15782, 25322, 2196, 22723, 36179, 36598, 4379, 38138, 31186, 43381, 51518, 12454, 
    63013, 35066, 59934, 42314, 10058, 3420, 986, 12869, 34584, 9678, 4793, 36362, 64309, 10673, 10263, 10726, 
    5144, 47081, 29246, 33431, 39659, 57873, 7990, 19022, 4086, 9407, 4454, 12130, 50376, 58830, 33965, 55354, 
    14992, 28074, 24136, 45616, 61891, 14279, 49349, 34068, 49812, 57729, 15563, 61584, 1779, 36799, 58357, 10944, 
    27869, 21423, 37198, 54197, 25669, 40721, 63252, 54295, 53307, 62593, 12001, 43937, 48208, 53710, 44631, 21631, 
    2274, 7877, 18593, 54935, 59071, 29488, 64785, 2214, 56325, 34640, 63928, 57490, 10573, 23159, 3802, 29106, 
    28908, 21411, 59926, 32411, 46546, 43558, 54512, 16911, 50413, 26907, 3648, 57059, 63721, 7666, 33313, 49517, 
    7208, 2219, 28908, 8505, 63675, 61614, 61153, 64472, 45419, 46037, 24334, 4201, 29062, 35585, 35400, 51530, 
    39414, 62063, 41368, 57595, 8040, 53317, 64068, 15155, 59817, 44326, 47518, 44944, 52214, 34710, 49456, 33001, 
    59444, 8101, 55851, 11472, 50080, 30103, 39719, 23858, 26342, 47846, 62595, 57593, 39959, 63423, 41559, 50939, 
    58884, 14848, 6637, 54281, 27818, 47651, 61391, 19434, 20460, 36025, 36214, 41969, 39102, 10563, 27388, 24186, 
    5260, 15487, 65165, 59592, 51825, 8857, 36067, 3528, 44339, 53889, 59730, 9647, 19979, 51110, 31931, 35991, 
    57555, 64769, 1255, 23024, 35039, 49009, 63571, 45557, 17909, 62334, 59485, 29978, 48049, 36585, 65136, 25513, 
    57782, 58541, 25297, 47681, 59811, 59253, 5338, 2185, 37513, 609, 12998, 1355, 51922, 12506, 62396, 50478, 
    53515, 27922, 19675, 44584, 56264, 57134, 58790, 55285, 31795, 32017, 4282, 29949, 39030, 7214, 22530, 17306, 
    38023, 28534, 27204, 3553, 17019, 16002, 46471, 43178, 7811, 8469, 5110, 48185, 36448, 54681, 6955, 16814, 
    3547, 43028, 18182, 46474, 8539, 34341, 3007, 6639, 30732, 46573, 39410, 13429, 40553, 45820, 20633, 52743, 
    11667, 15814, 38976, 56901, 53652, 38672, 150, 61901, 51936, 26170, 57242, 44197, 15884, 11039, 56435, 21407, 
    21309, 26226, 50037, 10752, 45108, 42160, 53828, 24808, 7334, 56327, 46739, 37684, 48571, 49393, 36378, 52489, 
    43932, 35069, 44953, 62599, 41140, 53683, 12384, 49249, 14521, 27292, 51831, 26212, 2956, 13176, 2809, 41888, 
    16499, 27518, 11261, 57964, 22349, 13443, 52126, 1604, 2607, 34722, 61169, 39467, 39399, 42196, 60420, 14369, 
    57993, 3200, 53218, 2136, 27200, 38972, 12897, 52173, 13827, 11430, 40229, 61673, 43356, 14084, 59873, 24917, 
    2293, 14899, 59434, 56950, 38173, 36752, 71, 8314, 14067, 54186, 25403, 20845, 17156, 19433, 28061, 12610, 
    37190, 43101, 44930, 23183, 62704, 15974, 49039, 25574, 55362, 24378, 46374, 22653, 57485, 49187, 28470, 26258, 
    24441, 29049, 24924, 4765, 28854, 39749, 19974, 26650, 6620, 31253, 19163, 1067, 57961, 13783, 44803, 10118, 
    63853, 32942, 58419, 47611, 32583, 39794, 22891, 34486, 52227, 17936, 19181, 22816, 57408, 6116, 13545, 19842, 
    23037, 47628, 58086, 3403, 5148, 949, 12671, 1723, 30721, 15950, 60288, 46913, 32024, 5117, 34738, 43650, 
    39827, 27496, 7261, 53518, 16338, 33370, 52700, 33412, 19838, 56487, 3761, 47891, 59040, 30412, 30760, 10080, 
    63032, 57426, 32023, 26678, 8295, 60649, 366, 12215, 21238, 3289, 9472, 47800, 31607, 22155, 15517, 29548, 
    12154, 21250, 17300, 54400, 45637, 21859, 38026, 18864, 17300, 17033, 44373, 34067, 5030, 3660, 16956, 28831, 
    18631, 44484, 62232, 50722, 41690, 49387, 48943, 38406, 50663, 25722, 39668, 16216, 19019, 1267, 22761, 9293, 
    26970, 10031, 54330, 48444, 6480, 53781, 14884, 7009, 43436, 62559, 53383, 40845, 21513, 18178, 28467, 22957, 
    57525, 403, 45642, 22155, 19987, 42474, 60368, 58528, 65351, 4777, 8495, 64328, 5914, 44968, 60881, 9294, 
    57961, 1295, 22458, 15618, 64524, 55483, 52069, 59001, 51125, 54822, 21043, 48677, 43548, 18952, 22110, 59548, 
    2114, 45637, 13682, 54011, 14302, 6530, 40600, 6804, 52367, 59174, 20481, 18454, 444, 32497, 64781, 48362, 
    20363, 39348, 51228, 7309, 37967, 57041, 45205, 15923, 22461, 35746, 4428, 26899, 15566, 32045, 52826, 24763, 
    33946, 6200, 59578, 13607, 25039, 43272, 49701, 11342, 33907, 65230, 46374, 5280, 2838, 32188, 29268, 31903, 
    10872, 23636, 57719, 48782, 27314, 59464, 6180, 11883, 62036, 6609, 25430, 18955, 4790, 12754, 27360, 19197, 
    46015, 15710, 62881, 20019, 10153, 36406, 51809, 29090, 65262, 28612, 19952, 16155, 62969, 14607, 25927, 14714, 
    17696, 27423, 65388, 59705, 36073, 39081, 5187, 37790, 58863, 30364, 26109, 6846, 42745, 64992, 44438, 28081, 
    42913, 38584, 48828, 42000, 33009, 61475, 39672, 41873, 46049, 56423, 24882, 46670, 34309, 23822, 28487, 45062, 
    14867, 64160, 63942, 18974, 22180, 65303, 51706, 52096, 41443, 53179, 29366, 54435, 8301, 33638, 46919, 16263, 
    34859, 25048, 52544, 43966, 64413, 61396, 37767, 5255, 27120, 11852, 65250, 34104, 58018, 42484, 30556, 6244 };
  

  int i;
  for (i = 0; i<1000; i++)
    FFT(RealData, ImagData, 2048, 11, -1);

}
