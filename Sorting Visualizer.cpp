#include <SDL.h>
#include <iostream>
#include <ctime>
#include <string>
using namespace std;

// ------------------ SETTINGS ------------------
const int SCREEN_WIDTH = 910;
const int SCREEN_HEIGHT = 750;

const int arrSize = 130;
const int rectSize = 7;

// global delay in milliseconds (affects all sorts)
int SORT_DELAY = 20;

// ----------------------------------------------
int arr[arrSize];
int Barr[arrSize];

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool complete = false;
bool quitProgram = false;

// ------------------ SDL INIT ------------------
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Couldn't initialize SDL. SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Sorting Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Couldn't create window. SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Couldn't create renderer. SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// ------------------ VISUALIZATION ------------------
void visualize(int x = -1, int y = -1, int z = -1) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int j = 0;
    for (int i = 0; i <= SCREEN_WIDTH - rectSize; i += rectSize) {
        SDL_Rect rect = {i, SCREEN_HEIGHT - arr[j], rectSize, arr[j]};

        if (complete) {
            SDL_SetRenderDrawColor(renderer, 100, 180, 100, 0); // green for done
            SDL_RenderFillRect(renderer, &rect);
        } else if (j == x || j == z) {
            SDL_SetRenderDrawColor(renderer, 230, 57, 70, 0); // red for swap
            SDL_RenderFillRect(renderer, &rect);
        } else if (j == y) {
            SDL_SetRenderDrawColor(renderer, 65, 105, 225, 0); // blue for current
            SDL_RenderFillRect(renderer, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 170, 183, 184, 0); // gray
            SDL_RenderFillRect(renderer, &rect);
        }
        j++;
    }

    SDL_RenderPresent(renderer);
}

// ------------------ SORTING ALGORITHMS ------------------
void bubbleSort() {
    for (int i = 0; i < arrSize - 1; i++) {
        for (int j = 0; j < arrSize - 1 - i; j++) {
            if (arr[j + 1] < arr[j]) {
                swap(arr[j], arr[j + 1]);
                visualize(j, j + 1, arrSize - i);
                SDL_Delay(SORT_DELAY);
            }
        }
    }
}

void insertionSort() {
    for (int i = 1; i < arrSize; i++) {
        int j = i - 1;
        int temp = arr[i];
        while (j >= 0 && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
            visualize(i, j + 1);
            SDL_Delay(SORT_DELAY);
        }
        arr[j + 1] = temp;
    }
}

void selectionSort() {
    for (int i = 0; i < arrSize - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < arrSize; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
            visualize(i, minIndex);
            SDL_Delay(SORT_DELAY);
        }
        swap(arr[i], arr[minIndex]);
    }
}

int partition_array(int a[], int si, int ei) {
    int count_small = 0;
    for (int i = si + 1; i <= ei; i++) {
        if (a[i] <= a[si]) count_small++;
    }
    int c = si + count_small;
    swap(a[si], a[c]);
    visualize(c, si);

    int i = si, j = ei;
    while (i < c && j > c) {
        if (a[i] <= a[c]) i++;
        else if (a[j] > a[c]) j--;
        else {
            swap(a[i], a[j]);
            visualize(i, j);
            SDL_Delay(SORT_DELAY);
            i++; j--;
        }
    }
    return c;
}

void quickSort(int a[], int si, int ei) {
    if (si >= ei) return;
    int c = partition_array(a, si, ei);
    quickSort(a, si, c - 1);
    quickSort(a, c + 1, ei);
}

void merge2SortedArrays(int a[], int si, int ei) {
    int size_output = (ei - si) + 1;
    int* output = new int[size_output];

    int mid = (si + ei) / 2;
    int i = si, j = mid + 1, k = 0;

    while (i <= mid && j <= ei) {
        if (a[i] <= a[j]) {
            output[k++] = a[i++];
        } else {
            output[k++] = a[j++];
        }
        visualize(i, j);
    }
    while (i <= mid) output[k++] = a[i++];
    while (j <= ei) output[k++] = a[j++];

    for (int l = si, x = 0; l <= ei; l++, x++) {
        a[l] = output[x];
        visualize(l);
        SDL_Delay(SORT_DELAY);
    }
    delete[] output;
}

void mergeSort(int a[], int si, int ei) {
    if (si >= ei) return;
    int mid = (si + ei) / 2;
    mergeSort(a, si, mid);
    mergeSort(a, mid + 1, ei);
    merge2SortedArrays(a, si, ei);
}

void inplaceHeapSort(int* input, int n) {
    // build heap
    for (int i = 1; i < n; i++) {
        int child = i;
        while (child > 0) {
            int parent = (child - 1) / 2;
            if (input[child] > input[parent]) swap(input[child], input[parent]);
            else break;
            visualize(parent, child);
            SDL_Delay(SORT_DELAY);
            child = parent;
        }
    }
    // extract
    for (int heapLast = n - 1; heapLast > 0; heapLast--) {
        swap(input[0], input[heapLast]);
        int parent = 0;
        while (true) {
            int left = 2 * parent + 1, right = 2 * parent + 2, maxIndex = parent;
            if (left < heapLast && input[left] > input[maxIndex]) maxIndex = left;
            if (right < heapLast && input[right] > input[maxIndex]) maxIndex = right;
            if (maxIndex == parent) break;
            swap(input[parent], input[maxIndex]);
            visualize(maxIndex, parent, heapLast);
            SDL_Delay(SORT_DELAY);
            parent = maxIndex;
        }
    }
}

// ------------------ UTILITIES ------------------
void loadArr() {
    memcpy(arr, Barr, sizeof(int) * arrSize);
}

void randomizeAndSaveArray() {
    srand((unsigned)time(NULL));
    for (int i = 0; i < arrSize; i++) {
        Barr[i] = rand() % (SCREEN_HEIGHT - 50) + 10; // avoid full-height bars
    }
}

// ------------------ MAIN LOOP ------------------
void execute() {
    if (!init()) {
        cout << "SDL Initialization Failed.\n";
        return;
    }

    randomizeAndSaveArray();
    loadArr();

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                quitProgram = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_q: quit = true; quitProgram = true; break;
                    case SDLK_0: randomizeAndSaveArray(); loadArr(); complete = false; break;
                    case SDLK_1: loadArr(); selectionSort(); complete = true; break;
                    case SDLK_2: loadArr(); insertionSort(); complete = true; break;
                    case SDLK_3: loadArr(); bubbleSort(); complete = true; break;
                    case SDLK_4: loadArr(); mergeSort(arr, 0, arrSize - 1); complete = true; break;
                    case SDLK_5: loadArr(); quickSort(arr, 0, arrSize - 1); complete = true; break;
                    case SDLK_6: loadArr(); inplaceHeapSort(arr, arrSize); complete = true; break;
                }
            }
        }
        visualize();
    }
    close();
}

// ------------------ INTRO + CONTROLS ------------------
void showControls() {
    cout << "\nControls:\n"
         << " 0 : New randomized list\n"
         << " 1 : Selection Sort\n"
         << " 2 : Insertion Sort\n"
         << " 3 : Bubble Sort\n"
         << " 4 : Merge Sort\n"
         << " 5 : Quick Sort\n"
         << " 6 : Heap Sort\n"
         << " q : Quit\n";
}

int main() {
    cout << "================ Sorting Visualizer ================\n";
    cout << "Visualizes Selection, Insertion, Bubble, Merge, Quick, and Heap Sort using SDL2.\n";
    cout << "Array size: " << arrSize << " elements.\n";
    cout << "Press ENTER to start...";
    cin.ignore();

    while (!quitProgram) {
        showControls();
        execute();
    }

    cout << "\nEXITING PROGRAM.\n";
    return 0;
}
