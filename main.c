#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <dirent.h>

typedef struct {
    char *Name;
    int Price;
    int Stock;
    Texture2D Cover;
} Game;

typedef struct {
    char *Name;
    int SellCount;
    int SellPrice;
} SellInfo;

// Flags
Game *Games;
int GamesSize = 10;
char **ImgPathes;
int TotalImg = 0;
int PathCount = 0;
int ImageResizeHeight = 298;
int ImageResizeWidth = 598;
int RenderCount = 0;
int RenderRow = 0;
int RenderLastRow = 0;
Vector2 scroll = { 0.0f, 0.0f };
Rectangle bounds = { 400, 0, 1250, 800};
Rectangle view = { 0, 0, 2000, 2000 };
bool ShowAddWindow = false;
char NameAddBuffer[50] = {0};
char PriceAddBuffer[50] = {0};
char StockAddBuffer[50] = {0};
Texture2D ImgTexture = {0};
bool NameEditFlag= false;
bool PriceEditflag = false;
bool StockEditflag = false;
bool PicLoadFlag = false;
bool ImgLoadFlag = false;
int ShodwLoadWindow = false;
bool AddButtonState = true;
bool AddLabels = false;
Font FiraFont;

void Game_initializer();
void Game_Adder(Game game);
void Game_Remover(Game game);
int Game_IndexFinder(Game game);
void Game_Operations(char operation, Game game);
void add_path(char **pathes, char *path);
void Retrieve_FileNames();

void Game_initializer() {
    Games = (Game *)calloc(GamesSize, sizeof(Game));
}

void Game_Adder(Game game) {
    GamesSize++;
    RenderCount++;
    Games = (Game *)realloc(Games, GamesSize * sizeof(Game));
    Games[GamesSize - 1] = (Game){0};
    if (Games == NULL) {
        // handle the error => realloc returns NULL once it fails => it won't affect the
        // original memory
    }
    Games[(GamesSize - 1) - 10] = game;
}

void Game_Remover(Game game) {
    int GameIndex = Game_IndexFinder(game);
    if (GameIndex == -1) {
        // game not found => handle it with a gui or some other method later on
    }
    
    for (int i = GameIndex; i < GamesSize - 1; i++) {
        Games[i] = Games[i + 1];
    }
    
    GamesSize--;
    Games = (Game *)realloc(Games, GamesSize * sizeof(Game));
    if (Games == NULL) {
        // handle the error => realloc returns NULL once it fails => it won't affect the
        // original memory
    }

}
int Game_IndexFinder(Game game) {
    for (int i = 0; i < GamesSize; i++) {
        if (strcmp(game.Name, Games[i].Name) == 0) {
            return i;
        }
    }
    
    return -1;
}

void Game_Operations(char operation, Game game) {
    switch (operation) {
    case '+':
        Game_Adder(game);
        break;
    case '-':
        Game_Remover(game);
        break;
    default:
        // handle it
        break;
    }
}

Game SearchedGames[10] = {0};
int TotalFoundGames = 0;

void Game_Search(char *SearchedWord) {
    int SearchedWordLength = strlen(SearchedWord);
    printf("%s %d \n", SearchedWord, SearchedWordLength);
    for (int i = 0, SearchedGameIndex = 0; i < GamesSize; i++) {
        if (Games[i].Name != NULL) {
            for (int j = 0, CharCounter = 0; j < SearchedWordLength; j++) {
                if (Games[i].Name[j] == SearchedWord[j]) {
                    CharCounter++;
                    if (CharCounter == SearchedWordLength) {
                        SearchedGames[SearchedGameIndex] = Games[i];
                        SearchedGameIndex++;
                        TotalFoundGames++;
                        printf("Found \n");
                    }
                } else {
                    printf("Not Found \n");
                    break;
                }
            }
        }
    }
}

FILE *Sale_File;
void Sale_FileHandler() {
    Sale_File = fopen("Sale_Information.txt", "a");
    if (Sale_File == NULL) {
        // Handle error if file cannot be opened
    }
}

void Game_Sold(Game game, int SellCount) {
    int GameIndex = Game_IndexFinder(game);
    int TotalPrice = SellCount * Games[GameIndex].Price;
    Games[GameIndex].Stock -= SellCount;
    
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    
    fprintf(Sale_File, 
    "\n{ \n Name       : %s\n Count      : %d\n EachPrice  : %d \n TotalPrice : %d\n Date       : %d/%d/%d\n Time       : %02d:%02d \n}",
                                                Games[GameIndex].Name,
                                                SellCount,
                                                Games[GameIndex].Price,
                                                TotalPrice,
                                                tm_info->tm_year + 1900, 
                                                tm_info->tm_mon + 1,
                                                tm_info->tm_mday,
                                                tm_info->tm_hour,
                                                tm_info->tm_min);
}

void add_path(char **pathes , char *path)
{
	PathCount++;
	if (PathCount == 10) 
	{
		ImgPathes = (char **)realloc(pathes, PathCount * 2 * sizeof(char *));
	}
	pathes[PathCount - 1] = strdup(path);
}
 
void Retrieve_FileNames()
{
	DIR *CurrentDirectory;
	struct dirent *entry;
	char *Extention;
	TotalImg = 0;
	PathCount = 0;
	ImgPathes = (char **)calloc(10, sizeof(char *));
	CurrentDirectory = opendir("images");
	
	while ((entry = readdir(CurrentDirectory)) != NULL)
	{
		Extention = strrchr(entry->d_name, '.');
		if (strcmp(Extention , ".png") == 0)
		{
			add_path(ImgPathes, entry->d_name);
			TotalImg++;
		}
	}
	closedir(CurrentDirectory);
}  

void Render_Labels()
{
    DrawText("Name", 30, 435, 40, BLACK);
    DrawText("Price", 30, 490 , 40, BLACK);
    DrawText("Stock", 30, 550, 40, BLACK);
    DrawTextEx(FiraFont, "Name", (Vector2){30, 350}, 40, 0, WHITE);
}


Texture2D Preview_Image(char *ImgPath)
{
    char RelativePath[128];
    snprintf(RelativePath, 128, "images/%s", ImgPath);
    Image Img = LoadImage(RelativePath);
    ImageResize(&Img, ImageResizeWidth, ImageResizeHeight);
    Texture2D ImgTexture = LoadTextureFromImage(Img);
    
    UnloadImage(Img);
    return ImgTexture;
}

// 1 - when we press the add game => we should validate our inputs 
// 2 - when we close the add windows => we should free the current
// game object => game = {0} , prob does the job
// 3 - figure out a cool container , for showing each game , and their
// info , then write a for loop for rendering them in a well designed
// layout , 2 games in a row seems cool => done
// 4 - add the remove buttons , and other neccessery buttons =>
// test their functionality 
// 5 - after writing the Container and the layout , then write the
// scrollBar, remove the labels for scrollbar , also fix the content size
// of the scrollpanel , write it based on the total games and the height of
// the games , and scale it with that regard

int main()
{
    Game game = {0};
    Game_initializer();
    const int width = 1650;
    const int height = 800;
    Retrieve_FileNames();
    
    InitWindow(width, height, "Test");
    GuiLoadStyle("style_dark.rgs");
    SetTargetFPS(30);
    FiraFont = LoadFontEx("FiraCode.ttf" , 30 , NULL , 0);
    GuiSetFont(FiraFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    
    while(!WindowShouldClose())
    {
        Rectangle content = { 0, 0, 900, (RenderCount / 2  + 1) * 445};
        
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        
        GuiScrollPanel(bounds, "", content, &scroll, NULL);
        if (AddButtonState && GuiButton((Rectangle){0 , 0 , 200 , 50} , "ADD"))
        {
            ShowAddWindow = true;
            AddLabels = true;
        }
        
        if (ShowAddWindow == true) 
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "ADD WINDOW"))
            {
                ShowAddWindow = false;
            }
            if (GuiTextBox((Rectangle){85 , 430 , 310 , 50} , NameAddBuffer , 50, NameEditFlag )) {
                NameEditFlag = !NameEditFlag;
            }
            if (GuiTextBox((Rectangle){85 , 485, 310, 50} , PriceAddBuffer , 50, PriceEditflag)) {
                PriceEditflag = !PriceEditflag;
            }
            if (GuiTextBox((Rectangle){85 , 540, 310 , 50} , StockAddBuffer, 50, StockEditflag)) {
                StockEditflag = !StockEditflag;
            }
            GuiLabel((Rectangle){5, 430, 70, 50}, "Name");
            GuiLabel((Rectangle){5, 485, 70, 50}, "Price");
            GuiLabel((Rectangle){5, 540, 70, 50}, "Stock");
            
            if (GuiButton((Rectangle){195 , 595 , 200 , 50} , "Load Cover")) 
            {
                ShodwLoadWindow = true;
                AddLabels = false;
                AddButtonState = false;
            }
            if (GuiButton((Rectangle){0 , 595 , 195 , 50} , "Add Game"))
            {
                int PriceInt;
                int StockInt;
                
                sscanf(PriceAddBuffer, "%d", &PriceInt);
                sscanf(StockAddBuffer, "%d", &StockInt);
                game.Name = strdup(NameAddBuffer);
                game.Price = PriceInt;
                game.Stock = StockInt;
                Game_Adder(game);
            }
            if (ShodwLoadWindow == true)
            {
                if (GuiWindowBox((Rectangle){0, 0, 400, 800}, "Load Cover"))
                {
                    ShodwLoadWindow = false;
                    AddLabels = true;
                    AddButtonState = true;
                }
                for (int i = 0; i < TotalImg; i++)
                {
                    if (i == TotalImg - 1)
                    {
                        break;
                    }
                    if (GuiButton((Rectangle){0, i * 25 + 25, 400, 25}, ImgPathes[i]))
                    {
                        ImgTexture = Preview_Image(ImgPathes[i]);
                    }   
                }
                if (GuiButton((Rectangle){0, 575, 400, 25}, "Refresh"))
                {
                    Retrieve_FileNames();
                }
                if (GuiButton((Rectangle){0, 600, 400, 25}, "Add Cover"))
                {
                    game.Cover = ImgTexture;
                }
                DrawTexture(ImgTexture, 0 , 625, WHITE);
            }
        }
        
        RenderRow = (RenderCount / 2) + 1  ;
        RenderLastRow = RenderCount - ((RenderRow - 1) * 2);
        
        if (RenderCount > 0)
        {
            int GameIndex = 0;
            for (int i = 0 ; i < RenderRow; i++) 
            {
                if (i != RenderRow - 1)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        int x = 410 + ((j * 600) + (j * 15));
                        int y = 32 + (i * 445) + (i * 12);
                        x = x + scroll.x;
                        y = y + scroll.y;
                        char PriceBuffer[10];
                        char StockBuffer[10];
                        
                        snprintf(PriceBuffer, 20, "%d", Games[GameIndex].Price);
                        snprintf(StockBuffer, 20, "%d", Games[GameIndex].Stock);
                        
                        GuiButton((Rectangle){x, y, 600, 445}, "");
                        GuiButton((Rectangle){x, y + 300, 600, 75}, Games[GameIndex].Name);
                        GuiButton((Rectangle){x, y + 375, 300, 75}, PriceBuffer);
                        GuiButton((Rectangle){x + 300, y + 375, 300, 75}, StockBuffer);
                        GuiUnlock();
                        DrawTexture(Games[GameIndex].Cover, x + 2, y + 2, WHITE);
                        GameIndex++;
                    }
                } else
                {
                    for (int j = 0; j < RenderLastRow; j++)
                    {
                        int x = 410 + ((j * 600) + (j * 15));
                        int y = 32 + (i * 445) + (i * 12);
                        x = x + scroll.x;
                        y = y + scroll.y;
                        char PriceBuffer[10];
                        char StockBuffer[10];
                        snprintf(PriceBuffer, 20, "%d", Games[GameIndex].Price);
                        snprintf(StockBuffer, 20, "%d", Games[GameIndex].Stock);
                        GuiButton((Rectangle){x, y, 600, 445}, "");
                        GuiButton((Rectangle){x, y + 300, 600, 75}, Games[GameIndex].Name);
                        GuiButton((Rectangle){x, y + 375, 300, 75}, PriceBuffer);
                        GuiButton((Rectangle){x + 300, y + 375, 300, 75}, StockBuffer);
                        GuiUnlock();
                        DrawTexture(Games[GameIndex].Cover, x + 2, y + 2, WHITE);
                        GameIndex++;
                    }
                }
            }
        }
            EndDrawing();
        }
    //UnloadTexture(ImgTexture);
    CloseWindow();
    return 0;
}