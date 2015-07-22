static FloatHi is_donor (CharPtr str, Int4 len)
{
  FloatHi one[4]={0.35, 0.35, 0.19, 0.11};
  FloatHi two[4]={0.59, 0.13, 0.14, 0.14};
  FloatHi three[4]={0.08, 0.02, 0.82, 0.08};
  FloatHi four[4]={0.01, 0.01, 1.00, 0.01};
  FloatHi five[4]={0.01, 0.01, 0.01, 1.00};
  FloatHi six[4]={0.51, 0.03, 0.43, 0.03};
  FloatHi seven[4]={0.71, 0.08, 0.12, 0.09};
  FloatHi eight[4]={0.06, 0.05, 0.84, 0.05};
  FloatHi nine[4]={0.15, 0.16, 0.17, 0.52};
  FloatHi score =1.000;
  Int4   i;
  Int4  PNTR num=NULL;

  if ((num = (Int4Ptr)MemNew(len*sizeof(Int4)))==NULL) {
    return(-1);
  }

  for (i = 0; i <= len; i++){
    if (str[i]=='A')
      num[i] = 0;
    if (str[i]=='C')
      num[i] = 1;
    if (str[i]=='G')
      num[i] = 2;
    if (str[i]=='T')
      num[i] = 3;
  }
  score *= one[num[0]];
  score *= two[num[1]];
  score *= three[num[2]];
  score *= four[num[3]];
  score *= five[num[4]];
  score *= six[num[5]];
  score *= seven[num[6]];
  score *= eight[num[7]];
  score *= nine[num[8]];

  MemFree(num);
  num=NULL;

  return score;
}



static Int4 getSplicePos (CharPtr str)
{
  Int4     offset = -1;
  Int4     xcursor = 0;
  Int4     c;
  FloatHi  topscore = -FLT_MAX,
           score;
  Char     tmpstr[9];
  Int4     length;

  if (str == NULL)
    return -1;
  length = MIN(StringLen(str)/2-10, 10);
  while (xcursor <= length)
  {
      for (c = 0; c < 9; c++)
      {
        tmpstr[c] = str[xcursor+c];
      }
      if ((score=is_donor(tmpstr, 8)) > topscore)
      {
        topscore = score;
        offset = xcursor;
      }
      xcursor += 1;
  }
  if (topscore > 0.000010 && offset >=0)
    return offset+3;
  return -1;
}

