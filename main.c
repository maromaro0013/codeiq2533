#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (1)

#define cTILE_PATTERNS_MAX    (4)
#define cFIELD_SIZE           (10)
#define cFIELD_TILE_BUFF_SIZE (13)        // 104bits(100bits使う)

#define cFIELD_TILE_HASH_MAX  (4*4*16*16) // w(2bits)*h(2bits)*x(4bits)*y(4bits)

#define get_tile_buff(buff, y, x)      ( (buff[(y*cFIELD_SIZE + x)/8] >> (x%8)) & 1 )
#define set_tile_buff(buff, y, x)      ( (buff[(y*cFIELD_SIZE + x)/8] |= 1 << (x%8)) )

typedef struct TILE_t {
  char w;
  char h;
  char x;
  char y;

  unsigned short tile_hash;
}TILE;

typedef struct FIELD_INFO_t {
  char w;
  char h;

  char padd[2];
}FIELD_INFO;

typedef struct POS_t {
  char x;
  char y;
  char padd[2];
}POS;

typedef struct FIELD_t {
  char tiles_count;
  char size_amount;
  unsigned short empty_cnt;

  char tile_buff[cFIELD_SIZE*cFIELD_SIZE];

  POS empty[cFIELD_SIZE*cFIELD_SIZE];
}FIELD;

FIELD_INFO g_filed_info;
char g_tile_limit_dp[cFIELD_TILE_HASH_MAX + 1];

static const TILE g_tile_patterns[cTILE_PATTERNS_MAX] = {
  {1, 1, 0, 0},
  {2, 2, 0, 0},
  {4, 2, 0, 0},
  {4, 4, 0, 0},
};

void init_field(FIELD* f, int h, int w) {
  f->tiles_count = f->size_amount = 0;
  f->empty_cnt = 0;
  memset((void*)f->tile_buff, 0, sizeof(f->tile_buff));

  int i =0;
  int j = 0;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      f->empty[f->empty_cnt].x = j;
      f->empty[f->empty_cnt].y = i;
      f->empty_cnt++;
    }
  }
}

int chk_tile_limit(TILE* p) {
  FIELD_INFO* info = &g_filed_info;

  if (p->y + p->h > info->h) {
    return FALSE;
  }
  if (p->x + p->w > info->w) {
    return FALSE;
  }
  return TRUE;
}

unsigned short create_tile_hash(TILE* p) {
  unsigned short ret;
  ret = ((p->w - 1) << 10 ) | ((p->h -1) << 8) | (p->x << 4) | p->y;
  p->tile_hash = ret;

  return ret;
}

int chk_fill_field(FIELD* f) {
  FIELD_INFO* info = &g_filed_info;

  if (f->size_amount >= info->w*info->h) {
    return TRUE;
  }
  return FALSE;
}

int chk_tile_placement(FIELD* f, TILE* p) {
  FIELD_INFO* info = &g_filed_info;
  int i = 0;
  int x, y;
  TILE* target = NULL;

  if (g_tile_limit_dp[p->tile_hash] == -1) {
    g_tile_limit_dp[p->tile_hash] = chk_tile_limit(p);
  }
  if (g_tile_limit_dp[p->tile_hash] == FALSE) {
    return FALSE;
  }

  for (y = p->y; y < p->y + p->h; y++) {
    for (x = p->x; x < p->x + p->w; x++) {
      if (get_tile_buff(f->tile_buff, y, x)) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

int tile_placement(FIELD* f, TILE* p) {
  FIELD_INFO* info = &g_filed_info;
  int x, y;
  int i, j;

  f->tiles_count++;
  f->size_amount += p->w*p->h;

  for (y = p->y; y < (p->y + p->h); y++) {
    for (x = p->x; x < (p->x + p->w); x++) {
      set_tile_buff(f->tile_buff, y, x);
    }
  }

  POS pos;
  for (i = 0; i < f->empty_cnt; i++) {
    pos = f->empty[i];
    if ((pos.y >= p->y && pos.y < (p->y + p->h)) && (pos.x >= p->x && pos.x < (p->x + p->w))) {
      for (j = i; j < f->empty_cnt - 1; j++) {
        f->empty[j] = f->empty[j + 1];
      }
      i--;
/*
      if (i+1 < f->empty_cnt) {
        memcpy((void*)&f->empty[i], (void*)&f->empty[i+1], sizeof(POS)*(f->empty_cnt - i - 1));
        i--;
      }
*/
      f->empty_cnt--;
    }
  }

  return TRUE;
}

int solve_field(FIELD* f, int pattern_max) {
  int pattern = 0;
  int ret = 0;
  TILE tile;
  FIELD next_field;
  FIELD_INFO* info = &g_filed_info;

/*
  for (tile.y = 0; tile.y < info->h; tile.y++) {
    for (tile.x = 0; tile.x < info->w; tile.x++) {
      if (!get_tile_buff(f->tile_buff, tile.y, tile.x)) {
        goto exit_loop;
      }
    }
  }

exit_loop:
*/
  tile.x = f->empty[0].x;
  tile.y = f->empty[0].y;
  for (pattern = 0; pattern < pattern_max; pattern++) {
    tile.w = g_tile_patterns[pattern].w;
    tile.h = g_tile_patterns[pattern].h;
    create_tile_hash(&tile);

    if (pattern == 0 || chk_tile_placement(f, &tile)) {
      memcpy((void*)&next_field, (void*)f, sizeof(next_field));
      //next_field = *f;
      tile_placement(&next_field, &tile);

      if (chk_fill_field(&next_field) == TRUE) {
        ret += 1;
      }
      else {
        ret += solve_field(&next_field, pattern_max);
      }
    }
  }
  return ret;
}

int main (int argc, char** argv) {
  char buff[256];
  char tmpbuff[256];
  FIELD field;
  int ret = 0;

  fgets(buff, sizeof(buff), stdin);

  int i = 0;
  int j = 0;
  for (i = 0; i < strlen(buff); i++){
    if (buff[i] == ',') {
      tmpbuff[i] = '\0';
      break;
    }
    tmpbuff[i] = buff[i];
  }
  g_filed_info.w = atoi(tmpbuff);
  i += 1;

  for (j = 0; i < strlen(buff); j++, i++){
    if (buff[i] == ',') {
      tmpbuff[j] = '\0';
      break;
    }
    tmpbuff[j] = buff[i];
  }
  g_filed_info.h = atoi(tmpbuff);

  memset((void*)g_tile_limit_dp, -1, sizeof(g_tile_limit_dp));

  memset((void*)&field, 0, sizeof(field));
  init_field(&field, g_filed_info.h, g_filed_info.w);
  ret = solve_field(&field, cTILE_PATTERNS_MAX);
  printf("%d\n", ret);

  return 0;
}
