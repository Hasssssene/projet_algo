#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "raymath.h"
#include <math.h>

#define MAX_CHILDREN 5 // Maximum number of children for each node

typedef struct NaryTreeNode {
    int data;
    int childCount;
    struct NaryTreeNode* children[MAX_CHILDREN];
} NaryTreeNode;

NaryTreeNode* root = NULL; // Root of the N-ary tree
int inputValue = 0;
int parentValue = 0;
int levelValue = 1;
int searchValue = 0;
int searchResult = -1;

void InsertNode(NaryTreeNode** node, int value, int parentValue, int level) {
    if (*node == NULL) {
        *node = (NaryTreeNode*)malloc(sizeof(NaryTreeNode));
        (*node)->data = value;
        (*node)->childCount = 0;
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            (*node)->children[i] = NULL;
        }
    } else if ((*node)->data == parentValue && level > 1) {
        InsertNode(&((*node)->children[(*node)->childCount]), value, parentValue, level - 1);
        (*node)->childCount++;
    } else {
        for (int i = 0; i < (*node)->childCount; ++i) {
            InsertNode(&((*node)->children[i]), value, parentValue, level);
        }
    }
}


void DeleteTree(NaryTreeNode* node) {
    if (node != NULL) {
        for (int i = 0; i < node->childCount; ++i) {
            DeleteTree(node->children[i]);
        }
        free(node);
    }  
}


void DeleteNode(NaryTreeNode** node, int value) {
    if (*node != NULL) {
        for (int i = 0; i < (*node)->childCount; ++i) {
            if ((*node)->children[i]->data == value) {
                // Node found, delete it and its children
                DeleteTree((*node)->children[i]);
                // Remove the pointer to the deleted child
                for (int j = i; j < (*node)->childCount - 1; ++j) {
                    (*node)->children[j] = (*node)->children[j + 1];
                }
                (*node)->childCount--;
                return;
            } else {
                // Recursively search in the children
                DeleteNode(&((*node)->children[i]), value);
            }
        }
    }
}




NaryTreeNode* currentNode = NULL; // Keep track of the current node during search

void SearchNode(NaryTreeNode** node, int value, int x, int y, int radius, float parentAngle) {
    if ((*node) != NULL) {
        currentNode = *node; // Update the current node during the search

        if ((*node)->data == value) {
            // Node found, mark it as green
            searchResult = 1;
            return;
        } else {
            // Mark the node as yellow during the search
            searchResult = 0;
             DrawCircle(x, y, radius, YELLOW);
        }

        for (int i = 0; i < (*node)->childCount; i++) {
             float angle = parentAngle + (180.0 / ((*node)->childCount)) * (i + 0.5); // Evenly distribute children around the parent
        float length = radius * 2;
        Vector2 position = { cos(DEG2RAD * angle), sin(DEG2RAD * angle) };
        position = Vector2Scale(position, length);
        position = Vector2Add(position, (Vector2){ (float)x, (float)y });
        
        
            SearchNode(&((*node)->children[i]), value, position.x, position.y, radius / 2, parentAngle);
            

            // If the value is found in a child, mark the current node as green
            if (searchResult == 1) {
                 DrawCircle(x, y, radius, GREEN);
                return;
            }
        }

        // Reset the searchResult only if none of the child nodes match the search value
        if (searchResult == 0) {
            searchResult = -1;
        }
    }
}




void DrawTreeNode(NaryTreeNode *node, int x, int y, int radius, float parentAngle) {
    if (node == NULL) {
        return;
    }
     
    DrawCircle(x, y, radius, LIGHTGRAY);
    DrawText(TextFormat(" %d", node->data), x - radius / 4, y - radius / 2, 40*3*radius/200, BLACK);

    for (int i = 0; i < node->childCount; i++) {
        if (node->children[i] == NULL) {
            printf("Child %d of node %d is NULL\n", i, node->data);
            break;
        }

        float angle = parentAngle + (180.0 / (node->childCount)) * (i + 0.5); // Evenly distribute children around the parent
        float length = radius * 2;
        Vector2 position = { cos(DEG2RAD * angle), sin(DEG2RAD * angle) };
        position = Vector2Scale(position, length);
        position = Vector2Add(position, (Vector2){ (float)x, (float)y });
        DrawLine(x, y, position.x, position.y, BLACK);
        
        DrawTreeNode(node->children[i], position.x, position.y, radius / 2, parentAngle);
      
    }
}

int main() {
    const int screenWidth = 1800;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "N-ary Tree");

    Rectangle valueBox = {10, screenHeight - 80, 120, 30};
    Rectangle parentBox = {140, screenHeight - 80, 120, 30};
    Rectangle levelBox = {270, screenHeight - 80, 120, 30};
    Rectangle insertButton = {10, screenHeight - 40, 120, 30};
    Rectangle deleteButton = {140, screenHeight - 40, 120, 30};
    Rectangle resetButton = {270, screenHeight - 40, 120, 30};
    Rectangle searchButton = {530, screenHeight - 80, 120, 30};


    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if ((CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (  IsKeyPressed(32))) {
            InsertNode(&root, inputValue, parentValue, levelValue);
            inputValue =  0; // Reset input values
            levelValue++;
        }

        if (CheckCollisionPointRec(mousePos, resetButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            DeleteTree(root);
            root = NULL;
        }

        if (CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            DeleteNode(&root, inputValue);
            inputValue = 0;
        }
         if ((CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyPressed(KEY_ENTER))) {
           searchResult = -1; // Reset search result
            currentNode = NULL; // Reset current node
            SearchNode(&root, inputValue, screenWidth / 2, screenHeight / 3, 70, 0);
        }
        
        int key = GetKeyPressed();
        if ((key >= KEY_ZERO) && (key <= KEY_NINE)) {
            // Check which input box is selected
            if (CheckCollisionPointRec(mousePos, valueBox)) {
                inputValue = inputValue * 10 + (key - KEY_ZERO);
            } else if (CheckCollisionPointRec(mousePos, parentBox)) {
                parentValue = parentValue * 10 + (key - KEY_ZERO);
            } else if (CheckCollisionPointRec(mousePos, levelBox)) {
                levelValue = levelValue * 10 + (key - KEY_ZERO);
            }
        } else if ((key == KEY_BACKSPACE)) {
            if (CheckCollisionPointRec(mousePos, valueBox) ) {
               if (inputValue <= 10) {
                    inputValue=0;
                }
                inputValue /= 10;
 
            } else if (CheckCollisionPointRec(mousePos, parentBox) ) {
                parentValue /= 10;
            } else if (CheckCollisionPointRec(mousePos, levelBox) ) {
                levelValue /= 10;
            }
        }


        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(valueBox, GRAY);
        DrawText("Value :", valueBox.x + 10, valueBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", inputValue), valueBox.x + 80, valueBox.y +7, 20, BLACK);

        DrawRectangleRec(parentBox, GRAY);
        DrawText("Parent :", parentBox.x + 10, parentBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", parentValue), parentBox.x + 80, parentBox.y+7 , 20, BLACK);

        DrawRectangleRec(levelBox, GRAY);
        DrawText("Level :", levelBox.x + 10, levelBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", levelValue), levelBox.x + 80, levelBox.y+7 , 20, BLACK);

        DrawRectangleRec(insertButton, GRAY);
        DrawText("Insert Node", insertButton.x + 10, insertButton.y + 10, 15, BLACK);
        DrawRectangleRec(deleteButton, GRAY);
        DrawText("delete Tree", deleteButton.x + 10, deleteButton.y + 10, 15, BLACK);
        DrawRectangleRec(resetButton, GRAY);
        DrawText("Reset Tree", resetButton.x + 10, resetButton.y + 10, 15, BLACK);
        
        


        DrawRectangleRec(searchButton, GRAY);
        DrawText("Search", searchButton.x + 10, searchButton.y + 10, 15, BLACK);
      

        DrawTreeNode(root, screenWidth / 2, screenHeight / 3, 70, 0);

        // Highlight the path taken during the search
        if (searchResult != -1) {
            if (searchResult == 0) {
                DrawText("Not Found!", screenWidth / 2 - 60, 50, 20, RED);
            }
        }





        EndDrawing();
    }

    CloseWindow();

    return 0;
}
