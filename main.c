
// weighted graph using the adjacency list
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Include the header for bool type
#include <limits.h>  // Include the header for INT_MAX
#define MAX_VERTICES 1000
int distances[MAX_VERTICES]; // Array to store the distances from the source vertex

// Struct for a node in the adjacency list
typedef struct Node
{
    int dest;
    int weight;
    struct Node *next;
} Node;

// Struct for the graph
typedef struct Graph
{
    int numVertices; // Number of vertices in the graph
    char **cityNames;
    Node **adjLists; // the neighbors list.
} Graph;

// used within BFS
typedef struct Queue
{
    int front, rear;
    int items[MAX_VERTICES];
} Queue;

// Function to create an empty graph
Graph *createGraph()
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->numVertices = 0;
    graph->cityNames = NULL;
    graph->adjLists = NULL;
    return graph;
}

// Function to create a new node
Node *createNode(int dest, int weight)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Function to find the index of a city in the graph
int findCityIndex(Graph *graph, const char *cityName)
{
    for (int i = 0; i < graph->numVertices; i++)
    {
        if (strcmp(graph->cityNames[i], cityName) == 0)
        {
            return i;
        }
    }
    return -1;
}
// add edge to graph
void addEdge(Graph *graph, int src, int dest, int weight)
{
    Node *newNode = createNode(dest, weight);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}
// add city to graph
int addCity(Graph *graph, const char *cityName)
{
    int index = findCityIndex(graph, cityName);
    if (index != -1)
    {
        return index;
    }
    index = graph->numVertices;
    graph->cityNames = (char **)realloc(graph->cityNames, (index + 1) * sizeof(char *));
    graph->adjLists = (Node **)realloc(graph->adjLists, (index + 1) * sizeof(Node *));
    graph->cityNames[index] = strdup(cityName);
    graph->adjLists[index] = NULL;
    graph->numVertices++;
    return index;
}
// Print graph data
void printGraph(Graph *graph)
{
    for (int i = 0; i < graph->numVertices; i++)
    {
        Node *current = graph->adjLists[i];
        printf("From %s:\n", graph->cityNames[i]);
        if (current == NULL)
        {
            printf("  No outgoing edges.\n");
        }
        else
        {
            while (current != NULL)
            {
                printf("  To %s, Distance: %dkm\n", graph->cityNames[current->dest], current->weight);
                current = current->next;
            }
        }
        printf("\n");
    }
}
// read cities from file and store in graph(along with edges and weights)
void readFile(Graph *graph)
{
    // Read the data from the file
    FILE *fp = fopen("cities.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[100];
    while (fgets(line, 100, fp) != NULL)
    {
        // Split the line into the source city, destination city, and weight
        char *src = strtok(line, "\t");
        char *dest = strtok(NULL, "\t");
        int weight = atoi(strtok(NULL, " "));

        // Add the edge to the graph
        int srcIndex = addCity(graph, src);
        int destIndex = addCity(graph, dest);
        addEdge(graph, srcIndex, destIndex, weight);
    }

    fclose(fp);
}

// check if graph is empty
bool isGraphEmpty(Graph *graph)
{
    return (graph->numVertices == 0);
}

// print a given city neighbors
void printNeighbors(Graph *graph, int cityIndex)
{
    // Get the head node of the neighbors list for the current city.
    Node *current = graph->adjLists[cityIndex];

    // Iterate through the neighbors of the current city.
    while (current != NULL)
    {
        int neighborCity = current->dest;
        int weight = current->weight;

        // Print the neighbor city and weight.
        printf("%s (%dKM) \n", graph->cityNames[neighborCity], weight);

        // Move to the next neighbor in the linked list.
        current = current->next;
    }
}

// create empty queue
Queue *createQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

// check if queue is empty
bool isEmpty(Queue *queue)
{
    return queue->front == -1;
}

void enqueue(Queue *queue, int value)
{
    if (queue->rear == MAX_VERTICES - 1)
    {
        printf("Queue is full\n");
    }
    else
    {
        if (queue->front == -1)
        {
            queue->front = 0;
        }
        queue->rear++;
        queue->items[queue->rear] = value;
    }
}

int dequeue(Queue *queue)
{
    int item;
    if (isEmpty(queue))
    {
        printf("Queue is empty\n");
        item = -1;
    }
    else
    {
        item = queue->items[queue->front];
        queue->front++;
        if (queue->front > queue->rear)
        {
            queue->front = queue->rear = -1;
        }
    }
    return item;
}
char bfsOutput[10000] = ""; // Initialize the global variable

// usually for unweighted graphs which usually leads to a less accurate outcome/ bc it considers the first encountered path as the shortest path
// uses queues as helper functions
void bfs(Graph *graph, int sourceIndex, int destIndex)
{
    int *distances = (int *)malloc(sizeof(int) * graph->numVertices);
    int *visited = (int *)malloc(sizeof(int) * graph->numVertices);
    int *parents = (int *)malloc(sizeof(int) * graph->numVertices);

    for (int i = 0; i < graph->numVertices; i++)
    {
        distances[i] = INT_MAX;
        visited[i] = 0;
        parents[i] = -1;
    }

    distances[sourceIndex] = 0;
    visited[sourceIndex] = 1;

    Queue *queue = createQueue();
    enqueue(queue, sourceIndex);

    while (!isEmpty(queue))
    {
        int currentCityIndex = dequeue(queue);

        Node *neighbor = graph->adjLists[currentCityIndex];
        while (neighbor != NULL)
        {
            int neighborIndex = neighbor->dest;
            if (!visited[neighborIndex])
            {
                visited[neighborIndex] = 1;
                distances[neighborIndex] = distances[currentCityIndex] + neighbor->weight;
                parents[neighborIndex] = currentCityIndex;
                enqueue(queue, neighborIndex);
            }
            neighbor = neighbor->next;
        }
    }

    if (!visited[destIndex])
    {
        sprintf(bfsOutput, "No path found from the source city to the destination city.\n");

        // printf("No path found from the source city to the destination city.\n");
    }
    else
    {
        sprintf(bfsOutput, "Shortest path from %s to %s using BFS:\n", graph->cityNames[sourceIndex], graph->cityNames[destIndex]);

        // printf("Shortest path from %s to %s using BFS:\n", graph->cityNames[sourceIndex], graph->cityNames[destIndex]);
        // printf("%s ", graph->cityNames[destIndex]);
        sprintf(bfsOutput + strlen(bfsOutput), "%s ", graph->cityNames[destIndex]);

        int parent = parents[destIndex];
        while (parent != -1)
        {
            sprintf(bfsOutput + strlen(bfsOutput), "<- %s ", graph->cityNames[parent]);

            // printf("<- %s ", graph->cityNames[parent]);
            parent = parents[parent];
        }
        sprintf(bfsOutput + strlen(bfsOutput), "\nDistance: %d\n", distances[destIndex]);

        // printf("\nDistance: %d\n", distances[destIndex]);
    }

    free(distances);
    free(visited);
    free(parents);
    free(queue);
}

char dijkstraOutput[10000] = ""; // Initialize the global variable

// Finds the actual shotest path relying on the weights
// can be implemented using priority queues
void dijkstra(Graph *graph, int sourceIndex, int destIndex)
{
    int numVertices = graph->numVertices;
    // int distances[numVertices]; // Array to store the distances from the source vertex
    int parents[numVertices];  // Array to store the parent vertex of each vertex in the shortest path
    bool visited[numVertices]; // Array to keep track of visited vertices

    // Initialize distances, parents, and visited arrays
    for (int i = 0; i < numVertices; i++)
    {
        distances[i] = INT_MAX; // Set distances to infinity
        parents[i] = -1;        // Set parents to -1 (indicating no parent)
        visited[i] = false;     // Mark all vertices as not visited
    }

    distances[sourceIndex] = 0; // Distance from source to itself is 0

    // Loop through all vertices
    for (int count = 0; count < numVertices - 1; count++)
    {
        // Find the vertex with the minimum distance from the set of unvisited vertices
        int minDistance = INT_MAX;
        int minIndex = -1;
        for (int v = 0; v < numVertices; v++)
        {
            if (!visited[v] && distances[v] <= minDistance)
            {
                minDistance = distances[v];
                minIndex = v;
            }
        }

        // Mark the selected vertex as visited
        visited[minIndex] = true;

        // Update the distances of the adjacent vertices if shorter path is found
        Node *neighbor = graph->adjLists[minIndex];
        while (neighbor != NULL)
        {
            int neighborIndex = neighbor->dest;
            int newDistance = distances[minIndex] + neighbor->weight;
            if (!visited[neighborIndex] && newDistance < distances[neighborIndex])
            {
                distances[neighborIndex] = newDistance;
                parents[neighborIndex] = minIndex;
            }
            neighbor = neighbor->next;
        }
    }
    // // Create a queue to store the path
    // Queue *pathQueue = createQueue();

    // // Traverse the parent vertices and enqueue them to the path queue
    // int currentIndex = destIndex;
    // while (currentIndex != -1)
    // {
    //     enqueue(pathQueue, currentIndex);
    //     currentIndex = parents[currentIndex];
    // }

    // return pathQueue;

    // Uncomment the following lines for direct print of distance // hae to edit the code to void

    // Print the shortest path from source to destination
    // printf("Shortest path from %s to %s: ", graph->cityNames[sourceIndex], graph->cityNames[destIndex]);
    sprintf(dijkstraOutput, "Shortest path from %s to %s using Dijkstra:\n", graph->cityNames[sourceIndex], graph->cityNames[destIndex]);

    int currentIndex = destIndex;
    while (currentIndex != -1)
    {
        sprintf(dijkstraOutput + strlen(dijkstraOutput), "<- %s ", graph->cityNames[currentIndex]);

        // printf("<- %s ", graph->cityNames[currentIndex]);
        currentIndex = parents[currentIndex];
    }
    printf("\n");
    sprintf(dijkstraOutput + strlen(dijkstraOutput), "\nDistance: %d\n", distances[destIndex]);

    // printf("Total distance: %d\n", distances[destIndex]);
}

// print result to file
void printToFile()
{
    // Print the output to a file
    FILE *outputFile = fopen("shortest_distance.txt", "a");
    if (outputFile != NULL)
    {
        fprintf(outputFile, "%s", bfsOutput);
        fprintf(outputFile, "%s", dijkstraOutput);

        fclose(outputFile);
    }
    else
    {
        printf("Error opening the output file.\n");
    }
}
// main menu
void menu()
{
    printf("1-Load cities\n");
    printf("2-Enter source\n");
    printf("3-Enter destination\n");
    printf("4-Save solution to file and continue\n");
    printf("5-Save solution to file and Exit\n");
}
int main()
{
    int x = -1;
    Graph *graph = createGraph();
    char sourceCity[100]; // Declare as a char array
    int sourceIndex = -1, destIndex = -1;
    // Queue *shortestPathQueue; //expermintal 
    do
    {
        menu();
        printf("Please enter your operation:\n");
        scanf("%d", &x);

        switch (x)
        {
        case 1: //load city //data 
            readFile(graph);
            printGraph(graph);
            break;
        case 2: //get source city

            if (isGraphEmpty(graph))
            {
                printf("No cities loaded. Please load cities first.\n");
            }
            else
            {
                printf("Please enter the source city:\n");
                scanf("%s", sourceCity);
                sourceIndex = findCityIndex(graph, sourceCity);
                if (sourceIndex == -1)
                {
                    printf("Source city '%s' not found.\n", sourceCity);
                }else{
                printf("city %s and index %d\n", sourceCity, sourceIndex);
                printf("Direct neighbors\n");
                printNeighbors(graph, sourceIndex);
                }
            }
            break;
        case 3: //find shortest distance using two algorithms and insert destination
            if (isGraphEmpty(graph))
            {
                printf("No cities loaded. Please load cities first.\n");
            }
            else if (sourceIndex == -1)

            {
                printf("Please enter the source city first.\n");
            }
            else
            {
               
                printf("Please enter the destination city:\n");
                scanf("%s", sourceCity);
                destIndex = findCityIndex(graph, sourceCity);
                if (destIndex == -1)
                {
                    printf("Destination city '%s' not found.\n", sourceCity);
                }
                else
                {
                    bfs(graph, sourceIndex, destIndex); //bfs solution 
                    // Print the output to the terminal
                    printf("%s", bfsOutput);

                    // Find the shortest path using Dijkstra's algorithm and get the path queue
                    // shortestPathQueue = dijkstra(graph, sourceIndex, destIndex);
                    dijkstra(graph, sourceIndex, destIndex); //dijkstra solution
                    // Print the output to the terminal
                    printf("%s", dijkstraOutput);

                    // // Print the shortest path
                    // printf("Shortest path from %s to %s:\n ", graph->cityNames[sourceIndex], graph->cityNames[destIndex]);

                    // while (!isEmpty(shortestPathQueue))
                    // {
                    //     int vertex = dequeue(shortestPathQueue);
                    //     printf("<-%s ", graph->cityNames[vertex]);
                    // }
                    // printf("\n");
                    // printf("Total distance: %d\n", distances[destIndex]);
                    // // dijkstra(graph, sourceIndex, destIndex);

                    // printf("\n");
                }
            }
            break;
        case 4: //print to file and continue
            printToFile();
            break;

        case 5: //print to file and exit
            printToFile();
            printf("Exiting Program.\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    } while (x != 5);

    return 0;
}