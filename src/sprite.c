#include "sprite.h"
#include "simple_logger.h"
#include "graphics3d.h"

#include <SDL_image.h>

#define MaxSprites    1024

static Sprite SpriteList[MaxSprites];


void InitSpriteList()
{
  int x;
  memset(SpriteList,0,sizeof(Sprite) * MaxSprites);
  for(x = 0;x < MaxSprites;x++)SpriteList[x].image = NULL;
  atexit(CloseSprites);
}


Sprite *SpriteNew()
{
    int i;
    for (i = 0;i < MaxSprites;i++)
    {
        if (!SpriteList[i].used)
        {
            memset(&SpriteList[i],0,sizeof(Sprite));
            SpriteList[i].used = 1;
            return &SpriteList[i];
        }
    }
    return NULL;
}

Sprite *SpriteGetByFilename(char *filename)
{
    int i;
    for (i = 0;i < MaxSprites;i++)
    {
        if  ((SpriteList[i].used > 0)
            &&(strcmp(SpriteList[i].filename,filename) == 0))
        {
            return &SpriteList[i];
        }
    }
    return NULL;
}

Sprite *LoadSprite(char *filename,int fw, int fh)
{
    Sprite *sprite;
    SDL_Surface *image;
    
    sprite = SpriteGetByFilename(filename);
    if (sprite)return sprite;
    
    sprite = SpriteNew();
    if (!sprite)return NULL;
    
    image = IMG_Load(filename);
    if (!image)
    {
        slog("failed to load sprite image file: %s, re: %s",filename, SDL_GetError());
        return NULL;
    }
    sprite->image = image;
    if (fw == -1)
    {
        sprite->w = sprite->image->w;
    }
    if (fh == -1)
    {
        sprite->h = sprite->image->h;
    }
    /*set the rest of the data*/
    
    return sprite;
}

void DeleteSprite(Sprite *sprite)
{
    if (!sprite)return;
    if (sprite->image != NULL)
    {
        SDL_FreeSurface(sprite->image);
    }
    memset(sprite,0,sizeof(Sprite));
}

void FreeSprite(Sprite *sprite)
{
  /*first lets check to see if the sprite is still being used.*/
  if (!sprite)return;
  sprite->used--;
  if(sprite->used <= 0)
  {
      DeleteSprite(sprite);
  }
}

void CloseSprites()
{
  int i;
  for(i = 0;i < MaxSprites;i++)
  {
      DeleteSprite(&SpriteList[i]);
  }
}


/*eol@eof*/
