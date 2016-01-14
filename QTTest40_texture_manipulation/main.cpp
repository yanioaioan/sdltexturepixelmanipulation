/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 2000;
const int SCREEN_HEIGHT = 2000;
TTF_Font *gFont;


//Texture wrapper class
class LTexture
{
    public:
        //Initializes variables
        LTexture();

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile( std::string path );

        #ifdef _SDL_TTF_H
        //Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor , int x, int y);
        #endif

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );

        //Renders texture at given point
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        void update( );

        //Gets image dimensions
        int getWidth();
        int getHeight();

        //Pixel manipulators
        bool lockTexture();
        bool unlockTexture();
        void* getPixels();
        int getPitch();

        //The actual hardware texture
        SDL_Texture* mTexture;

    private:

        void* mPixels;
        int mPitch;

        //Image dimensions
        int mWidth;
        int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gFooTexture;
SDL_Surface* formattedSurface;
SDL_Surface* gScreenSurface;
LTexture::LTexture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    mPixels = NULL;
    mPitch = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}
bool LTexture::loadFromFile( std::string path )
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {       
        //Convert surface to display format

//        formattedSurface = SDL_ConvertSurface( loadedSurface,  SDL_GetWindowSurface(gWindow)->format, NULL );
        formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_ARGB8888, NULL );

        if( formattedSurface == NULL )
        {
            printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {


            //Create blank streamable texture
            newTexture = SDL_CreateTexture( gRenderer,SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
            if( newTexture == NULL )
            {
                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
            }
            else
            {
                //Lock texture for manipulation
                SDL_LockTexture( newTexture, NULL, &mPixels, &mPitch );

                //Copy loaded/formatted surface pixels
                memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

                //Unlock texture to update
                SDL_UnlockTexture( newTexture );
                mPixels = NULL;

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;
            }

//            //Get rid of old formatted surface
//            SDL_FreeSurface( formattedSurface );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}




#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor,int x, int y )
{

    /*Create 100 surfaces blit one to the existing one*/

    //Get rid of preexisting texture
//    free();

     gFont = TTF_OpenFont("arial.ttf", 10);
    //Render text surface

     SDL_Surface* textSurface;
//     for(int i=0;i<100;i++)
     {
     textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
//    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, textureText.c_str(), textColor ,300);

        SDL_Rect textLocation = { x, y, 8, 10 };
    //SDL_BlitSurface(textSurface, NULL, formattedSurface, &textLocation);





    //Then create a texture from the "final textSurface"

    if( textSurface != NULL )
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;


            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

            //do not clear the screen
//            SDL_RenderClear( gRenderer );

            SDL_SetTextureColorMod(mTexture,textColor.r,textColor.g,textColor.b);
            SDL_RenderCopy(gRenderer,mTexture, NULL, /*&charLocation*/  &textLocation);



        }


    }
    else
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }



     }
    TTF_CloseFont(gFont);
    //Get rid of old surface
    SDL_FreeSurface( textSurface );

    //Return success
    return mTexture != NULL;
}
#endif

void LTexture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
    }
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
    //Set blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

//void LTexture::update()
//{
//    SDL_UpdateTexture((SDL_Texture*)this, NULL, formattedSurface->pixels, formattedSurface->pitch);
//}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );




}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

bool LTexture::lockTexture()
{
    bool success = true;

    //Texture is already locked
    if( mPixels != NULL )
    {
        printf( "Texture is already locked!\n" );
        success = false;
    }
    //Lock texture
    else
    {
        if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
        {
            printf( "Unable to lock texture! %s\n", SDL_GetError() );
            success = false;
        }
    }

    return success;
}

bool LTexture::unlockTexture()
{
    bool success = true;

    //Texture is not locked
    if( mPixels == NULL )
    {
        printf( "Texture is not locked!\n" );
        success = false;
    }
    //Unlock texture
    else
    {
        SDL_UnlockTexture( mTexture );
        mPixels = NULL;
        mPitch = 0;
    }

    return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

int LTexture::getPitch()
{
    return mPitch;
}

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {

            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }


                //Initialize SDL_ttf
                if( TTF_Init() == -1 )
                { return false; }



            }
        }
    }

    return success;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

int powerof(int x,int y)
{
    int tmp=x;
    for(int i=1;i<y;i++)
    {

        tmp*=x;
    }
    return tmp;
}


std::string getGrayShade(int avRGBVal)
{
    std::string asciichar = " ";


    if (avRGBVal >= 225)
    {
        asciichar = " ";
    }
    else if (avRGBVal >= 200)
    {
        asciichar = ".";
    }
    else if (avRGBVal >= 175)
    {
        asciichar = ":";
    }
    else if (avRGBVal >= 150)
    {
        asciichar = "!";
    }
    else if (avRGBVal >= 125)
    {
        asciichar = "o";
    }

    else if (avRGBVal >= 100)
    {
        asciichar = "*";
    }
    else if (avRGBVal >= 75)
    {
        asciichar = "8";
    }
    else if (avRGBVal >= 50)
    {
        asciichar = "%";
    }
    else if (avRGBVal >= 25)
    {
        asciichar = "#";
    }
    else
    {
        asciichar = "@";
    }

    return asciichar;
}


bool loadMedia()
{
    //Loading success flag
    bool success = true;


    //Load foo' texture
    if( !gFooTexture.loadFromFile( "antipaxoi.png" ) )
    {
        printf( "Failed to load corner texture!\n" );
        success = false;
    }
    else
    {
        //Lock texture
        if( !gFooTexture.lockTexture() )
        {
            printf( "Unable to lock Foo' texture!\n" );
        }
        //Manual color key
        else
        {
            //Get pixel data
            Uint32* pixels = (Uint32*)gFooTexture.getPixels();//formattedSurface->pixels;
            int pixelCount = ( gFooTexture.getPitch() / 4 ) * gFooTexture.getHeight();


            //Map colors
//            Uint32 colorKey = SDL_MapRGB( SDL_GetWindowSurface( gWindow )->format, 0, 0xFF, 0xFF );
//            Uint32 transparent = SDL_MapRGBA( SDL_GetWindowSurface( gWindow )->format, 0xFF, 0xFF, 0xFF, 0x00 );

//            Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
//            Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0XFF, 0XFF, 0XFF, 0xFF );

//                        for( int i = 0; i < pixelCount; ++i )
//                        {
//                            if( pixels[ i ] == colorKey )
//                            {
//                                pixels[ i ] = transparent;
//                            }
//                        }



            //Averagine by block
            //Color key pixels
            int powerof2=powerof(2,10);//for a formattedSurface->w/64 block pixel width this should produce 8 blocks of 8 pixels
            int block_width=formattedSurface->w/powerof2;
            int block_height=formattedSurface->h/powerof2;

            std::string blockCharacterString;
            //For each block every block_width pixels
            for(size_t bx = block_width; bx <= formattedSurface->w;  bx += block_width)
            {
                //For each block every block_height pixels
                for(size_t by = block_height; by <= formattedSurface->h; by += block_height)
                {

                    //total sum of Red,Green,Blue channel values
                    float sumR=0;
                    float sumG=0;
                    float sumB=0;

                    //CurrentPixel
                    Uint32 pixel;

                    int blockPixelcounter=0;
                    //For each block every block_height pixels
                    for(size_t x = bx-block_width; x < bx; x++)
                    {
                        for(size_t y = by-block_height; y < by; y++)
                        {

                            Uint8 red ;
                            Uint8 green ;
                            Uint8 blue ;
                            pixel=pixels[x + y * formattedSurface->w];

                            SDL_GetRGB( pixel, formattedSurface->format ,  &red, &green, &blue );


                            sumR=sumR+red;
                            sumG=sumG+green;
                            sumB= sumB+blue;
                            blockPixelcounter++;


                        }

                    }//square block end


                    //get the average chanel color
                    int avR=sumR/blockPixelcounter; int avG=sumG/blockPixelcounter; int avB=sumB/blockPixelcounter;

                    //Gray Scale it - based on each rgb value
//                    Uint8 v = 0.212671f * red  + 0.715160f * green  + 0.072169f * blue;

                    //Gray Scale it - based on the average rgb value
                    Uint8 v = 0.212671f * avR  + 0.715160f * avG  + 0.072169f * avB;
                    //pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;



                    /*
                     * instead of modifying pixel in the line above,
                      we can query the character which is going to replace this block based
                      on the average grayscale value of the 3 R,G,B channels
                     */
                    int avRGBValue=(avR+avG+avB)/3;
//                    blockCharacterString+=getGrayShade(avRGBValue);




//                    //iterate and replace all pixel color values with this average grayscal value
//                    for(size_t x = bx-block_width; x < bx; x++)
//                    {
//                        for(size_t y = by-block_height; y < by; y++)
//                        {
//                            //manually change the color of the pixel
//                                    pixel = (0xFF << 24) | (red << 16) | (green << 8) | blue;
//                            pixels [(formattedSurface->w* y)+x ] = pixel;
//                        }

//                    }//square block end replacing values



//                            //Avegage Block Color
//                            sumR+=red; sumG+=green; sumB+=blue;
//                            pixels [(gFooTexture.getWidth()* by)+bx ] = 255;


                    //Create a texture (appropriate chosen character, appropriately colored) for each block of pixels at bx,by to be replaced
                    SDL_Color AV_BLOCK_COLOR = {avR,avG,avB};

                    /*
                     * and now we can create a texture of the characters in the blockCharacterString
                     */
                    gFooTexture.loadFromRenderedText(getGrayShade(avRGBValue),AV_BLOCK_COLOR,bx,by);

                }

                //when we reach the end of each row of pixels we add a new line \n
                //blockCharacterString+="\n";

                //Render Current width offset
                SDL_RenderPresent(gRenderer);
            }

            //At the end. Get pixels of the surface associated with the gRenderer (formattedSurface)
            //save the image
            Uint32 * finaltexturePixels = (Uint32 *)formattedSurface->pixels;
            SDL_RenderReadPixels(gRenderer, &formattedSurface->clip_rect, formattedSurface->format->format, finaltexturePixels, formattedSurface->w * formattedSurface->format->BytesPerPixel);
            SDL_Surface* saveSurface = SDL_CreateRGBSurfaceFrom(finaltexturePixels, formattedSurface->w, formattedSurface->h,
                                                                formattedSurface->format->BitsPerPixel,
                                                                formattedSurface->w * formattedSurface->format->BytesPerPixel,
                                                                formattedSurface->format->Rmask, formattedSurface->format->Gmask,
                                                                formattedSurface->format->Bmask, formattedSurface->format->Amask);


            SDL_SaveBMP(saveSurface, "MYASCIICOLOREDIMAGE.bmp");


            //Get rid of old formatted surface
            SDL_FreeSurface( formattedSurface );

            //Unlock texture
            gFooTexture.unlockTexture();
        }
    }

    return success;
}

void close()
{
    //Free loaded images
    gFooTexture.free();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {

        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }

//                Clear screen
//                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

//                SDL_RenderClear( gRenderer );

//                //Render stick figure
//                gFooTexture.render( ( SCREEN_WIDTH - gFooTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gFooTexture.getHeight() ) / 2 );

//                //Update screen
//                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
