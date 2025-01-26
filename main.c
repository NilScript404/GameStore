#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <dirent.h>

typedef struct
{
    char *Name;
    int Price;
    int Stock;
    Texture2D Cover;
} Game;

typedef struct
{
    char *Name;
    int SellCount;
    int SellPrice;
} SellInfo;

typedef struct
{
    char name[50];
    int price;
    int stock;
    char ImgPath[50];
} Game_Save;

Game *Games = NULL;
int GamesSize = 10;
char **ImgPathes;
char **DbPathes;

int TotalDb = 0;
int DBCount = 0;

int TotalImg = 0;
int PathCount = 0;
int ImageIndex = 0;
int RenderCount = 0;
int RenderRow = 0;
int RenderLastRow = 0;
Vector2 scroll = {0.0f, 0.0f};
Rectangle bounds = {400, 0, 1250, 800};
Rectangle view = {0, 0, 2000, 2000};
bool ShowAddWindow = false;
bool ShowRemoveWindow = false;
bool OrderButtonState = true;
bool ShowOrderWindow = false;
bool ShowOrderState = false;
bool ShowDBCreationLabel = false;
bool ShowAddCover = false;
char NameAddBuffer[50] = {0};
char PriceAddBuffer[50] = {0};
char StockAddBuffer[50] = {0};
char NameRemoveBuffer[50] = {0};
char SearchUpdateBuffer[50] = {0};
char NameUpdateBuffer[50] = {0};
char PriceUpdateBuffer[50] = {0};
char StockUpdateBuffer[50] = {0};
char SearchOrderBuffer[50] = {0};
char StockOrderBuffer[50] = {0};
char DBNameBuffer[50] = {0};
bool ShowDbLoadWindow = false;
bool ShowDBLoadLabel = false;
bool LoadDbEditFlag = false;
char DBLoadBuffer[50] = {0};
char *DBLoadLabeled = "DB has been loaded";
int SaveImage = 0;
Game SearchedGames[10] = {0};
int TotalFoundGames = 0;
char SavePath[512] = {0};

char CreatedLabel[50] = "DB Created Successfully";
Game gameFound = {0};
Texture2D ImgTexture = {0};
Texture2D PreviewTexture = {0};
Texture2D CoverTexture = {0};
bool NameEditFlag = false;
bool PriceEditflag = false;
bool StockEditflag = false;
bool RemoveEditFlag = false;
bool UpdateEditFlag = false;
bool OrderEditFlag = false;
bool DBNameEditFlag = false;
bool PicLoadFlag = false;
bool ImgLoadFlag = false;
bool ShowRemoveState = false;
bool ShowUpdateWindow = false;
bool ShowUpdateState = false;
bool showCreateDBWindow = false;
int ShodwLoadWindow = false;
bool ShowFound = false;
bool ShowNotFound = false;
bool AddButtonState = true;
bool RemoveButtonState = true;
bool UpdateButtonState = true;
bool CreateButtonState = true;
bool LoadButtonState = true;
bool AddGameState = true;
bool AddLabels = false;

Game GameToRemove = {0};
Game GameToUpdate = {0};
Game GameToSell = {0};
Font FiraFont;

void Game_initializer();
void Game_Adder(Game game);
void Game_Remover(int index);
int Game_IndexFinder(Game game);
void add_path(char **pathes, char *path, char *pathName);
void Retrieve_FileNames(char ***PathArray, char *path, char *Type);

void Game_initializer()
{
    Games = (Game *)calloc(GamesSize, sizeof(Game));
}

void Game_Adder(Game game)
{
    GamesSize++;
    RenderCount++;
    Games = (Game *)realloc(Games, GamesSize * sizeof(Game));
    Games[GamesSize - 1] = (Game){0};
    Games[(GamesSize - 1) - 10] = game;
}

void Game_Remover(int index)
{
    if (index == -1)
    {
        ShowNotFound = true;
    }
    for (int i = index; i < GamesSize - 1; i++)
    {
        Games[i] = Games[i + 1];
    }
    GamesSize--;
    Games = (Game *)realloc(Games, GamesSize * sizeof(Game));
    if (Games == NULL)
    {
        // handle the error => realloc returns NULL once it fails => it won't affect the
        // original memory
    }
    RenderCount--;
}

int Game_IndexFinder(Game game)
{
    for (int i = 0; i < RenderCount; i++)
    {
        if (strcmp(game.Name, Games[i].Name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void Game_Search(char *SearchedWord)
{
    int SearchedWordLength = strlen(SearchedWord);
    printf("%s %d \n", SearchedWord, SearchedWordLength);
    for (int i = 0, SearchedGameIndex = 0; i < GamesSize; i++)
    {
        if (Games[i].Name != NULL)
        {
            for (int j = 0, CharCounter = 0; j < SearchedWordLength; j++)
            {
                if (Games[i].Name[j] == SearchedWord[j])
                {
                    CharCounter++;
                    if (CharCounter == SearchedWordLength)
                    {
                        SearchedGames[SearchedGameIndex] = Games[i];
                        SearchedGameIndex++;
                        TotalFoundGames++;
                        printf("Found \n");
                    }
                }
                else
                {
                    printf("Not Found \n");
                    break;
                }
            }
        }
    }
}

FILE *Sale_File;
void Sale_FileHandler()
{
    Sale_File = fopen("Sale_Informations.txt", "a");
    if (Sale_File == NULL)
    {
        printf("Sale_Information.txt Not Found");
    }
}

void Game_Sold(Game game, int SellCount)
{
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

void add_path(char **pathes, char *path, char *pathName)
{
    if (strcmp(pathName, "images") == 0)
    {
        PathCount++;
        if (PathCount == 10)
        {
            pathes = (char **)realloc(pathes, (PathCount) * 2 * sizeof(char *));
        }
        pathes[PathCount - 1] = strdup(path);
    }
    else
    {
        DBCount++;
        if (DBCount == 10)
        {
            pathes = (char **)realloc(pathes, (DBCount) * 2 * sizeof(char *));
        }
        pathes[DBCount - 1] = strdup(path);
    }
}

void Retrieve_FileNames(char ***PathArray, char *path, char *Type)
{
    DIR *Directory;
    struct dirent *entry;
    if (strcmp(path, "images") == 0)
    {
        TotalImg = 0;
        PathCount = 0;
    }
    else
    {
        TotalDb = 0;
        DBCount = 0;
    }
    char *Extention;
    *PathArray = (char **)calloc(10, sizeof(char *));
    Directory = opendir(path);
    if (!Directory)
    {
        perror("Opening dir failed");
        return;
    }
    
    while ((entry = readdir(Directory)) != NULL)
    {
        Extention = strrchr(entry->d_name, '.');
        if (strcmp(Extention, ".png") == 0)
        {
            add_path(*PathArray, entry->d_name, path);
            if (strcmp(path, "images") == 0)
            {
                TotalImg++;
            }
        }
        else if (strcmp(Extention, ".txt") == 0)
        {
            add_path(*PathArray, entry->d_name, path);
            if (strcmp(path, "DB") == 0)
            {
                TotalDb++;
            }
        }
    }
    closedir(Directory);
}

void Render_Labels()
{
    DrawText("Name", 30, 435, 40, BLACK);
    DrawText("Price", 30, 490, 40, BLACK);
    DrawText("Stock", 30, 550, 40, BLACK);
    DrawTextEx(FiraFont, "Name", (Vector2){30, 350}, 40, 0, WHITE);
}

Texture2D ImageToTexture(char *ImgPath, int ImageResizeWidth, int ImageResizeHeight)
{
    char RelativePath[128];
    snprintf(RelativePath, 128, "images/%s", ImgPath);
    Image Img = LoadImage(RelativePath);
    ImageResize(&Img, ImageResizeWidth, ImageResizeHeight);
    Texture2D ImgTexture = LoadTextureFromImage(Img);
    
    UnloadImage(Img);
    return ImgTexture;
}

void Render_Games()
{
    int GameIndex = 0;
    for (int i = 0; i < RenderRow; i++)
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
        }
        else
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
Game AddInput_Handler(Game game, char *NameAddBuffer, char *PriceAddBuffer, char *StockAddBuffer)
{
    int PriceInt;
    int StockInt;
    sscanf(PriceAddBuffer, "%d", &PriceInt);
    sscanf(StockAddBuffer, "%d", &StockInt);
    game.Name = strdup(NameAddBuffer);
    game.Price = PriceInt;
    game.Stock = StockInt;
    
    return game;
}
void RenderGameToRemoveLabels(Game game)
{
    char FoundName[200];
    char FoundPrice[128];
    char FoundStock[128];
    
    snprintf(FoundName, sizeof(FoundName), "Name  : %s", game.Name);
    snprintf(FoundPrice, sizeof(FoundPrice), "price : %d", game.Price);
    snprintf(FoundStock, sizeof(FoundStock), "Stock : %d ", game.Stock);
    GuiLabel((Rectangle){100, 560, 400, 50}, FoundName);
    GuiLabel((Rectangle){100, 590, 400, 50}, FoundPrice);
    GuiLabel((Rectangle){100, 620, 400, 50}, FoundStock);
}

void RenderGameToUpdate(Game game)
{
    char FoundName[256];
    char FoundPrice[128];
    char FoundStock[128];
    
    snprintf(FoundName, sizeof(FoundName), "Name  %s -->", game.Name);
    snprintf(FoundPrice, sizeof(FoundPrice), "price %d     -->", game.Price);
    snprintf(FoundStock, sizeof(FoundStock), "Stock %d     -->", game.Stock);
    GuiLabel((Rectangle){0, 560, 300, 50}, FoundName);
    GuiLabel((Rectangle){0, 600, 300, 50}, FoundPrice);
    GuiLabel((Rectangle){0, 640, 300, 50}, FoundStock);
}
void RenderGameToSell(Game game)
{
    char FoundName[256];
    char FoundPrice[128];
    char FoundStock[128];
    
    snprintf(FoundName, sizeof(FoundName), "Name  %s -->", game.Name);
    snprintf(FoundPrice, sizeof(FoundPrice), "price %d     -->", game.Price);
    snprintf(FoundStock, sizeof(FoundStock), "Stock %d     -->", game.Stock);
    GuiLabel((Rectangle){0, 560, 300, 50}, FoundName);
    GuiLabel((Rectangle){0, 600, 300, 50}, FoundPrice);
    GuiLabel((Rectangle){0, 640, 300, 50}, FoundStock);
}
void Game_Updater(Game game, char *name, char *price, char *stock)
{
    int Intprice = 0;
    int Intstock = 0;
    int index = Game_IndexFinder(game);
    sscanf(price, "%d", &Intprice);
    sscanf(stock, "%d", &Intstock);
    
    Games[index].Name = name;
    Games[index].Price = Intprice;
    Games[index].Stock = Intstock;
}

void Create_DB(char *name)
{
    char FilePath[50] = {0};
    snprintf(FilePath, 50, "DB/%s.txt", name);
    FILE *Games_DB = fopen(FilePath, "wb");
    fwrite(Games, sizeof(Game), RenderCount, Games_DB);
    fclose(Games_DB);
}

char ImagePathSave[50][256];
void Create_DBTEST(char *name)
{
    char FilePath[50] = {0};
    snprintf(FilePath, 50, "DB/%s.txt", name);
    FILE *Games_DB = fopen(FilePath, "wb");
     
    Game_Save *arr = (Game_Save *)calloc(RenderCount, sizeof(Game_Save));
    for (int i = 0; i < RenderCount; i++)
    {
        strcpy(arr[i].name, Games[i].Name);
        arr[i].price = Games[i].Price;
        arr[i].stock = Games[i].Stock;
        strcpy(arr[i].ImgPath, ImagePathSave[i]);
    }
    fwrite(arr, sizeof(Game_Save), RenderCount, Games_DB);
    fclose(Games_DB);
}

void Load_DBTEST(char *name)
{
    char FilePath[50];
    snprintf(FilePath, 50, "DB/%s", name);
    
    FILE *Game_DB = fopen(FilePath, "rb");
    fseek(Game_DB, 0, SEEK_END);
    int Game_DB_Size = ftell(Game_DB);
    rewind(Game_DB);
    int NewGameCount = Game_DB_Size / sizeof(Game_Save);
         
    RenderCount = NewGameCount;
    GamesSize = 10 + NewGameCount;
    
    if (Games != NULL)
    {
        free(Games);
    }
    
    Games = (Game *)calloc(GamesSize, sizeof(Game));
    
    Game_Save *GameSave = (Game_Save *)calloc(RenderCount, sizeof(Game_Save));
    fread(GameSave, sizeof(Game_Save), NewGameCount, Game_DB);
    
    for (int i = 0; i < NewGameCount; i++)
    {
        Games[i].Name = GameSave[i].name;
        Games[i].Price = GameSave[i].price;
        Games[i].Stock = GameSave[i].stock;
        Texture2D Cover = ImageToTexture(GameSave[i].ImgPath, 598, 298);
        Games[i].Cover = Cover;
    }
}

void Load_DB(char *name)
{
    char FilePath[50];
    snprintf(FilePath, 50, "DB/%s", name);
    FILE *Game_DB = fopen(FilePath, "rb");
    
    fseek(Game_DB, 0, SEEK_END);
    int Game_DB_Size = ftell(Game_DB);
    rewind(Game_DB);
    int NewGamesCount = Game_DB_Size / sizeof(Game);
     
    RenderCount = NewGamesCount;
    GamesSize = 10 + NewGamesCount;
    
    if (Games != NULL)
    {
        free(Games);
    }
    
    Games = (Game *)calloc(GamesSize, sizeof(Game));
    
    fread(Games, sizeof(Game), NewGamesCount, Game_DB);
    fclose(Game_DB);
}

void LoadCoverFirstState()
{
    AddLabels = true;
    AddButtonState = true;
    OrderButtonState = true;
    UpdateButtonState = true;
    RemoveButtonState = true;
    CreateButtonState = true;
    LoadButtonState = true;
    AddGameState = true;
}
void LoadCoverSecondState()
{
    AddLabels = false;
    AddButtonState = false;
    OrderButtonState = false;
    UpdateButtonState = false;
    RemoveButtonState = false;
    CreateButtonState = false;
    LoadButtonState = false;
    AddGameState = false;
}

int main()
{
    Sale_FileHandler();
    Game game = {0};
    Game_initializer();
    const int width = 1650;
    const int height = 800;
    Retrieve_FileNames(&ImgPathes, "images", "png");
    
    InitWindow(width, height, "Test");
    GuiLoadStyle("style_dark.rgs");
    SetTargetFPS(30);
    FiraFont = LoadFontEx("FiraCode.ttf", 30, NULL, 0);
    GuiSetFont(FiraFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
     
    while (!WindowShouldClose())
    {
        Rectangle content = {0, 0, 900, (RenderCount / 2 + 1) * 445};
        
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        
        for (int i = 0; i < DBCount; i++)
        {
            printf("%s \n", DbPathes[i]);
        }
        for (int i = 0; i < RenderCount; i++)
        {
            printf("%s %d %d \n", Games[i].Name, Games[i].Price, Games[i].Stock);
        }
        // DB CREATION
        if (CreateButtonState == true)
        {
            if (GuiButton((Rectangle){0, 250, 400, 50}, "CREATE"))
            {
                showCreateDBWindow = true;
            }
        }
        // DB LOAD
        if (LoadButtonState == true)
        {
            if (GuiButton((Rectangle){0, 200, 400, 50}, "Load DB"))
            {
                ShowDbLoadWindow = true;
            }
        }
        
        GuiScrollPanel(bounds, "", content, &scroll, NULL);
        if (AddButtonState && GuiButton((Rectangle){0, 0, 400, 50}, "ADD"))
        {
            ShowAddWindow = true;
            RemoveButtonState = false;
            AddLabels = true;
        }
        if (GuiButton((Rectangle){0, 50, 400, 50}, "Remove"))
        {
            if (RemoveButtonState == true)
                ShowRemoveWindow = true;
        }
        if (GuiButton((Rectangle){0, 100, 400, 50}, "Update"))
        {
            if (UpdateButtonState == true)
                ShowUpdateWindow = true;
        }
        if (ShowUpdateWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "Update Windows"))
            {
                ShowUpdateWindow = false;
                ShowFound = false;
                ShowNotFound = false;
            }
            GuiLabel((Rectangle){5, 430, 75, 50}, "Name");
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, SearchUpdateBuffer, 50, UpdateEditFlag))
            {
                UpdateEditFlag = !UpdateEditFlag;
            }
            if (GuiButton((Rectangle){0, 485, 400, 50}, "Search"))
            {
                ShowFound = false;
                ShowNotFound = false;
                Game gameSearch = {0};
                gameSearch.Name = SearchUpdateBuffer;
                int index = Game_IndexFinder(gameSearch);
                if (index != -1)
                {
                    ShowNotFound = false;
                    ShowFound = true;
                    GameToUpdate = Games[index];
                }
                else
                {
                    ShowNotFound = true;
                }
            }
            if (ShowNotFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Not Found!");
            }
            else if (ShowFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Found!");
                RenderGameToUpdate(GameToUpdate);
                
                char UpdateLabel[128];
                snprintf(UpdateLabel, sizeof(UpdateLabel), "Update %s", GameToUpdate.Name);
                if (GuiButton((Rectangle){0, 715, 400, 50}, UpdateLabel))
                {
                    Game_Updater(GameToUpdate, NameUpdateBuffer, PriceUpdateBuffer, StockUpdateBuffer);
                    ShowUpdateState = true;
                }
                if (ShowUpdateState == true)
                {
                    GuiLabel((Rectangle){45, 770, 400, 30}, "Game has been Updated");
                }
            }
            if (ShowFound && GuiTextBox((Rectangle){245, 567, 150, 35}, NameUpdateBuffer, 50, NameEditFlag))
            {
                NameEditFlag = !NameEditFlag;
            }
            if (ShowFound && GuiTextBox((Rectangle){245, 607, 150, 35}, PriceUpdateBuffer, 50, PriceEditflag))
            {
                PriceEditflag = !PriceEditflag;
            }
            if (ShowFound && GuiTextBox((Rectangle){245, 647, 150, 35}, StockUpdateBuffer, 50, StockEditflag))
            {
                StockEditflag = !StockEditflag;
            }
        }
        if (ShowAddWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "ADD WINDOW"))
            {
                ShowAddWindow = false;
                RemoveButtonState = true;
                ShowAddCover = true;
            }
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, NameAddBuffer, 50, NameEditFlag))
            {
                NameEditFlag = !NameEditFlag;
            }
            if (GuiTextBox((Rectangle){85, 485, 310, 50}, PriceAddBuffer, 50, PriceEditflag))
            {
                PriceEditflag = !PriceEditflag;
            }
            if (GuiTextBox((Rectangle){85, 540, 310, 50}, StockAddBuffer, 50, StockEditflag))
            {
                StockEditflag = !StockEditflag;
            }
            GuiLabel((Rectangle){5, 430, 70, 50}, "Name");
            GuiLabel((Rectangle){5, 485, 70, 50}, "Price");
            GuiLabel((Rectangle){5, 540, 70, 50}, "Stock");
            
            if (GuiButton((Rectangle){195, 595, 200, 50}, "Load Cover"))
            {
                ShodwLoadWindow = true;
                ShowAddCover = true;
                LoadCoverSecondState();
            }
            if (GuiButton((Rectangle){0, 595, 195, 50}, "Add Game"))
            {
                game = AddInput_Handler(game, NameAddBuffer, PriceAddBuffer, StockAddBuffer);
                Game_Adder(game);
            }
            
            if (ShodwLoadWindow == true)
            {
                
                if (GuiWindowBox((Rectangle){0, 0, 400, 800}, "Load Cover"))
                {
                    ShowAddCover = false;
                    ShodwLoadWindow = false;
                    LoadCoverFirstState();
                }
                for (int i = 0; i < TotalImg; i++)
                {
                    if (i == TotalImg - 1)
                    {
                        break;
                    }
                    if (GuiButton((Rectangle){0, i * 25 + 25, 400, 25}, ImgPathes[i]))
                    {
                        PreviewTexture = ImageToTexture(ImgPathes[i], 400, 200);
                        CoverTexture = ImageToTexture(ImgPathes[i], 598, 298);
                        strcpy(SavePath, ImgPathes[i]);
                        printf("%s", SavePath);
                    }
                }
                if (GuiButton((Rectangle){0, 575, 400, 25}, "Refresh"))
                {
                    Retrieve_FileNames(&ImgPathes, "images", "png");
                }
                if (ShowAddCover == true)
                {
                    if (GuiButton((Rectangle){0, 600, 400, 25}, "Add Cover"))
                    {
                        game.Cover = CoverTexture;
                        strcpy(ImagePathSave[SaveImage], SavePath);
                        SaveImage++;
                    }
                }
                DrawTexture(PreviewTexture, 0, 625, WHITE);
            }
        }
        if (ShowRemoveWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "Remove Windows"))
            {
                ShowRemoveWindow = false;
                ShowFound = false;
                ShowNotFound = false;
            }
            GuiLabel((Rectangle){5, 430, 75, 50}, "Name");
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, NameRemoveBuffer, 50, RemoveEditFlag))
            {
                RemoveEditFlag = !RemoveEditFlag;
            }
            if (GuiButton((Rectangle){0, 485, 400, 50}, "Search"))
            {
                gameFound.Name = NameRemoveBuffer;
                int index = Game_IndexFinder(gameFound);
                if (index != -1)
                {
                    ShowNotFound = false;
                    ShowFound = true;
                    GameToRemove = Games[index];
                }
                else
                {
                    ShowNotFound = true;
                }
            }
            if (ShowNotFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Not Found!");
            }
            else if (ShowFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Found!");
                RenderGameToRemoveLabels(GameToRemove);
                
                char RemoveLabel[128];
                snprintf(RemoveLabel, sizeof(RemoveLabel), "Remove %s", GameToRemove.Name);
                if (GuiButton((Rectangle){0, 715, 400, 50}, RemoveLabel))
                {
                    Game_Remover(Game_IndexFinder(GameToRemove));
                    ShowRemoveState = true;
                }
                if (ShowRemoveState == true)
                {
                    GuiLabel((Rectangle){45, 770, 400, 30}, "Game has been Removed");
                }
            }
        }
        if (OrderButtonState == true)
        {
            if (GuiButton((Rectangle){0, 150, 400, 50}, "Order"))
            {
                ShowOrderWindow = true;
            }
        }
        if (ShowOrderWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "Order Window"))
            {
                ShowOrderWindow = false;
                ShowFound = false;
                ShowNotFound = false;
            }
            GuiLabel((Rectangle){5, 430, 75, 50}, "Name");
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, SearchOrderBuffer, 50, OrderEditFlag))
            {
                OrderEditFlag = !OrderEditFlag;
            }
            if (GuiButton((Rectangle){0, 485, 400, 50}, "Search"))
            {
                ShowNotFound = false;
                ShowFound = false;
                ShowOrderState = false;
                Game gameSearch = {0};
                gameSearch.Name = SearchOrderBuffer;
                int index = Game_IndexFinder(gameSearch);
                if (index != -1)
                {
                    ShowNotFound = false;
                    ShowFound = true;
                    GameToSell = Games[index];
                }
                else
                {
                    ShowNotFound = true;
                }
            }
            if (ShowNotFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Not Found");
            }
            else if (ShowFound == true)
            {
                GuiLabel((Rectangle){100, 530, 400, 50}, "Game Found");
                RenderGameToSell(GameToSell);
                
                int count = 0;
                sscanf(StockOrderBuffer, "%d", &count);
                
                char OrderLabel[128];
                snprintf(OrderLabel, sizeof(OrderLabel), "Sell %d %s", count, GameToSell.Name);
                if (GuiButton((Rectangle){0, 715, 400, 50}, OrderLabel))
                {
                    Game_Sold(GameToSell, count);
                    ShowOrderState = true;
                }
            }
            if (ShowOrderState == true)
            {
                int count = 0;
                sscanf(StockOrderBuffer, "%d", &count);
                char SoldLabel[128];
                snprintf(SoldLabel, sizeof(SoldLabel), "Sold %d %s", count, GameToSell.Name);
                
                GuiLabel((Rectangle){100, 770, 400, 30}, SoldLabel);
            }
            if (ShowFound && GuiTextBox((Rectangle){245, 647, 150, 35}, StockOrderBuffer, 50, StockEditflag))
            {
                StockEditflag = !StockEditflag;
            }
        }
        if (showCreateDBWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "Create DB"))
            {
                showCreateDBWindow = false;
                ShowDBCreationLabel = false;
            }
             
            GuiLabel((Rectangle){5, 430, 75, 50}, "Name");
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, DBNameBuffer, 50, DBNameEditFlag))
            {
                DBNameEditFlag = !DBNameEditFlag;
            }
            if (GuiButton((Rectangle){0, 485, 400, 50}, "Create"))
            {
                Create_DBTEST(DBNameBuffer);
                ShowDBCreationLabel = true;
            }
            if (ShowDBCreationLabel == true)
            {
                char *tempNameBuff = strdup(DBNameBuffer);
                strcat(tempNameBuff, CreatedLabel);
                GuiLabel((Rectangle){0, 550, 400, 50}, tempNameBuff);
            }
        }
        if (ShowDbLoadWindow == true)
        {
            if (GuiWindowBox((Rectangle){0, 400, 400, 400}, "Load DB"))
            {
                ShowDbLoadWindow = false;
                ShowDBLoadLabel = false;
            }
            GuiLabel((Rectangle){5, 430, 75, 50}, "Name");
            if (GuiTextBox((Rectangle){85, 430, 310, 50}, DBLoadBuffer, 50, LoadDbEditFlag))
            {
                LoadDbEditFlag = !LoadDbEditFlag;
            }
            if (GuiButton((Rectangle){0, 485, 400, 50}, "Load"))
            {
                Retrieve_FileNames(&DbPathes, "DB", "txt");
                for (int i = 0; i < DBCount; i++)
                {
                    if (strcmp(DbPathes[i], DBLoadBuffer) == 0)
                    {
                        Load_DBTEST(DBLoadBuffer);
                        DBLoadLabeled = "DB Loaded";
                        break;
                    }
                    else
                    {
                        DBLoadLabeled = "DB Was Not Found";
                    }
                }
                ShowDBLoadLabel = true;
            }
            if (ShowDBLoadLabel == true)
            {
                char *tempNameBuff = strdup(DBLoadBuffer);
                strcat(tempNameBuff, DBLoadLabeled);
                GuiLabel((Rectangle){0, 550, 400, 50}, tempNameBuff);
            }
        }
        RenderRow = (RenderCount / 2) + 1;
        RenderLastRow = RenderCount - ((RenderRow - 1) * 2);
        if (RenderCount > 0)
        {
            Render_Games();
        }
        EndDrawing();
    }
     
    CloseWindow();
    return 0;
}