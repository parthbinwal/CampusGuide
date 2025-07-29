#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>

// Fix M_PI definition
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define A 6378137.0
#define F (1.0 / 298.257223563) // Fixed division
#define B (A * (1 - F))
#define V 15
#define INF 9999999
#define THRESHOLD 10000
#define WALKING_SPEED 1.4
#define STEP_LENGTH 0.8
#define MAX_NODES 15

typedef struct
{
  char name[50];
  char shortName[20];
  double lat;
  double lon;
} Node;

typedef struct
{
  int index;
  double fScore;
} PQNode;

typedef struct
{
  PQNode data[V];
  int size;
} PriorityQueue;

// Updated Node array with your college coordinates
// Converted from degrees/minutes/seconds to decimal degrees
Node nodes[V] =
    {
        {"main gate", "main gate", 29.375000, 79.531111},                 // 29°22'30"N 79°31'52"E
        {"oat", "oat", 29.375278, 79.530000},                             // 29°22'31"N 79°31'48"E
        {"basketball", "basketball", 29.375000, 79.530278},               // 29°22'30"N 79°31'49"E
        {"cricket ground", "cricket ground", 29.375000, 79.529444},       // 29°22'30"N 79°31'44"E
        {"bus", "bus", 29.374444, 79.531111},                             // 29°22'28"N 79°31'52"E
        {"DS", "DS", 29.375278, 79.529722},                               // 29°22'31"N 79°31'47"E
        {"academic block a", "academic block a", 29.375278, 79.530833},   // 29°22'31"N 79°31'51"E
        {"academic block c", "academic block c", 29.374444, 79.530556},   // 29°22'28"N 79°31'50"E
        {"academic block d", "academic block d", 29.374444, 79.530000},   // 29°22'28"N 79°31'48"E
        {"library", "library", 29.375556, 79.530556},                     // 29°22'32"N 79°31'50"E
        {"canteen", "canteen", 29.375000, 79.530833},                     // 29°22'30"N 79°31'51"E
        {"volleyball ground", "volleyball ground", 29.375000, 79.530000}, // 29°22'30"N 79°31'48"E
        {"hostel", "hostel", 29.375833, 79.529444},                       // 29°22'33"N 79°31'44"E
        {"saisandhya hall", "saisandhya hall", 29.375556, 79.530000},     // 29°22'32"N 79°31'48"E
        {"academic block b", "academic block b", 29.374722, 79.529722}    // 29°22'29"N 79°31'47"E
};

// Windows memory snapshot
void getMemorySnapshot(SIZE_T *memKB)
{
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex))
  {
    *memKB = (SIZE_T)(statex.ullAvailPhys / 1024); // Available RAM in KB
  }
  else
  {
    *memKB = 0;
  }
}

double toRadians(double degree)
{
  return degree * (M_PI / 180.0);
}

double vincenty(Node a, Node b)
{
  double U1 = atan((1 - F) * tan(toRadians(a.lat)));
  double U2 = atan((1 - F) * tan(toRadians(b.lat)));
  double L = toRadians(b.lon - a.lon);
  double lambda = L, lambda_prev;
  int iterations = 100;

  double sinSigma, cosSigma, sigma, sinAlpha, cos2Alpha, C;

  do
  {
    double sinLambda = sin(lambda), cosLambda = cos(lambda);
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);

    sinSigma = sqrt(pow(cosU2 * sinLambda, 2) +
                    pow(cosU1 * sinU2 - sinU1 * cosU2 * cosLambda, 2));
    if (sinSigma == 0)
      return 0;

    cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
    sigma = atan2(sinSigma, cosSigma);
    sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
    cos2Alpha = 1 - sinAlpha * sinAlpha;
    C = (F / 16) * cos2Alpha * (4 + F * (4 - 3 * cos2Alpha));

    lambda_prev = lambda;
    lambda = L + (1 - C) * F * sinAlpha *
                     (sigma + C * sinSigma * (cosSigma + C * cosSigma * (-1 + 2 * cosSigma * cosSigma)));
  } while (fabs(lambda - lambda_prev) > 1e-12 && --iterations);

  double u2 = cos2Alpha * (A * A - B * B) / (B * B);
  double A_coeff = 1 + (u2 / 16384) * (4096 + u2 * (-768 + u2 * (320 - 175 * u2)));
  double B_coeff = (u2 / 1024) * (256 + u2 * (-128 + u2 * (74 - 47 * u2)));

  double deltaSigma = B_coeff * sinSigma *
                      (cosSigma + (B_coeff / 4) * (cos(2 * sigma) - 3 * sinSigma * sinSigma));

  return B * A_coeff * (sigma - deltaSigma);
}

int findNodeByName(char *name)
{
  for (int i = 0; i < V; i++)
  {
    if (_stricmp(name, nodes[i].name) == 0 || _stricmp(name, nodes[i].shortName) == 0)
      return i;
  }
  return -1;
}

void reconstructPath(int src, int dest, int next[V][V], char *url, int distance, double timeRequired)
{
  strcat(url, "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=");
  char buffer[100];

  int u = src;
  while (u != dest)
  {
    sprintf(buffer, "%lf,%lf;", nodes[u].lat, nodes[u].lon);
    strcat(url, buffer);
    u = next[u][dest];
  }
  sprintf(buffer, "%lf,%lf", nodes[dest].lat, nodes[dest].lon);
  strcat(url, buffer);
  sprintf(buffer, "#map=19/%lf/%lf&distance=%d&time=%.2f", nodes[src].lat, nodes[src].lon, distance, timeRequired);
  strcat(url, buffer);
}

// Floyd War-shall Algorithm
void floydWarshall(int graph[V][V], int next[V][V])
{
  for (int k = 0; k < V; k++)
    for (int i = 0; i < V; i++)
      for (int j = 0; j < V; j++)
        if (graph[i][k] + graph[k][j] < graph[i][j])
        {
          graph[i][j] = graph[i][k] + graph[k][j];
          next[i][j] = next[i][k];
        }
}

// Dijkstra Algorithm
void dijkstra(int src, int dest, int *path, int *pathLength, double *totalDist)
{
  double dist[V];
  int visited[V] = {0}, prev[V];
  for (int i = 0; i < V; i++)
  {
    dist[i] = INF;
    prev[i] = -1;
  }
  dist[src] = 0;

  for (int i = 0; i < V - 1; i++)
  {
    double min = INF;
    int u = -1;
    for (int j = 0; j < V; j++)
      if (!visited[j] && dist[j] < min)
      {
        min = dist[j];
        u = j;
      }
    if (u == -1)
      break;

    visited[u] = 1;

    for (int v = 0; v < V; v++)
    {
      if (!visited[v] && u != v)
      {
        double d = vincenty(nodes[u], nodes[v]);
        if (d < THRESHOLD && dist[u] + d < dist[v])
        {
          dist[v] = dist[u] + d;
          prev[v] = u;
        }
      }
    }
  }

  if (dist[dest] == INF)
  {
    *pathLength = 0;
    *totalDist = INF;
    return;
  }

  int count = 0;
  for (int at = dest; at != -1; at = prev[at])
    path[count++] = at;
  for (int i = 0; i < count / 2; i++)
  {
    int tmp = path[i];
    path[i] = path[count - i - 1];
    path[count - i - 1] = tmp;
  }

  *pathLength = count;
  *totalDist = dist[dest];
}

// Priority queue functions for A*

void pqInsert(PriorityQueue *pq, int index, double fScore)
{
  int i = pq->size++;
  while (i > 0 && pq->data[(i - 1) / 2].fScore > fScore)
  {
    pq->data[i] = pq->data[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  pq->data[i].index = index;
  pq->data[i].fScore = fScore;
}

int pqExtractMin(PriorityQueue *pq)
{
  int min = pq->data[0].index;
  PQNode last = pq->data[--pq->size];
  int i = 0, child;
  while (i * 2 + 1 < pq->size)
  {
    child = i * 2 + 1;
    if (child + 1 < pq->size && pq->data[child + 1].fScore < pq->data[child].fScore)
      child++;
    if (last.fScore <= pq->data[child].fScore)
      break;
    pq->data[i] = pq->data[child];
    i = child;
  }
  pq->data[i] = last;
  return min;
}

void reconstructAStarPath(int cameFrom[], int current, Node nodes[V], char *url, int distance, double timeRequired)
{
  char buffer[100];
  strcat(url, "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=");
  int path[V], count = 0;
  while (current != -1)
  {
    path[count++] = current;
    current = cameFrom[current];
  }
  for (int i = count - 1; i >= 0; i--)
  {
    sprintf(buffer, "%lf,%lf;", nodes[path[i]].lat, nodes[path[i]].lon);
    strcat(url, buffer);
  }
  url[strlen(url) - 1] = '\0'; // remove trailing semicolon
  sprintf(buffer, "#map=19/%lf/%lf&distance=%d&time=%.2f", nodes[path[count - 1]].lat, nodes[path[count - 1]].lon, distance, timeRequired);
  strcat(url, buffer);
}

int aStar(int graph[V][V], Node nodes[V], int start, int goal, int *totalDist, int cameFrom[V])
{
  double gScore[V], fScore[V];
  int visited[V] = {0};
  PriorityQueue pq = {.size = 0};

  for (int i = 0; i < V; i++)
  {
    gScore[i] = INF;
    fScore[i] = INF;
    cameFrom[i] = -1;
  }

  gScore[start] = 0;
  fScore[start] = vincenty(nodes[start], nodes[goal]);

  pqInsert(&pq, start, fScore[start]);

  while (pq.size > 0)
  {
    int current = pqExtractMin(&pq);
    if (current == goal)
    {
      *totalDist = (int)gScore[goal];
      return 1;
    }

    visited[current] = 1;
    for (int neighbor = 0; neighbor < V; neighbor++)
    {
      if (graph[current][neighbor] == INF || visited[neighbor])
        continue;
      double tentative_gScore = gScore[current] + graph[current][neighbor];
      if (tentative_gScore < gScore[neighbor])
      {
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentative_gScore;
        fScore[neighbor] = tentative_gScore + vincenty(nodes[neighbor], nodes[goal]);
        pqInsert(&pq, neighbor, fScore[neighbor]);
      }
    }
  }
  return 0;
}

// JSON output functions for web integration
void printWebJSON(int choice, int distance, double timeMinutes, int steps, char *pathStr, char *algorithm)
{
  printf("{");
  printf("\"success\":true,");
  printf("\"algorithm\":\"%s\",", algorithm);
  printf("\"distance\":%d,", distance);
  printf("\"time\":%.2f,", timeMinutes);
  printf("\"steps\":%d,", steps);
  printf("\"path\":\"%s\"", pathStr);
  printf("}");
}

void printErrorJSON(char *message)
{
  printf("{\"success\":false,\"error\":\"%s\"}", message);
}

// Function to display all available locations
void displayAvailableLocations()
{
  printf("\n=== Available Campus Locations ===\n");
  printf("%-3s %-25s %-15s\n", "ID", "Location Name", "Short Name");
  printf("---------------------------------------------------\n");
  for (int i = 0; i < V; i++)
  {
    printf("%-3d %-25s %-15s\n", i, nodes[i].name, nodes[i].shortName);
  }
  printf("===================================\n\n");
}

int main(int argc, char *argv[])
{
  // Check if called from web
  bool webOutput = false;
  if (argc > 4 && strcmp(argv[4], "--web") == 0)
  {
    webOutput = true;
  }

  if (argc < 4)
  {
    if (webOutput)
    {
      printErrorJSON("Missing required parameters");
    }
    else
    {
      printf("Usage: %s <algorithm_choice> <source> <destination>\n", argv[0]);
      displayAvailableLocations();
    }
    return 1;
  }

  int choice = atoi(argv[1]);
  char *source = argv[2];
  char *dest = argv[3];

  int srcIdx = findNodeByName(source);
  int destIdx = findNodeByName(dest);

  if (srcIdx == -1 || destIdx == -1)
  {
    if (webOutput)
    {
      printErrorJSON("Invalid source or destination location");
    }
    else
    {
      printf("Invalid source or destination.\n");
      displayAvailableLocations();
    }
    return 1;
  }

  if (!webOutput)
  {
    printf("\n=== Campus Navigation System ===\n");
    printf("From: %s\n", nodes[srcIdx].name);
    printf("To: %s\n", nodes[destIdx].name);
  }

  // Initialize graph
  int graph[V][V], next[V][V];
  for (int i = 0; i < V; i++)
  {
    for (int j = 0; j < V; j++)
    {
      if (i == j)
        graph[i][j] = 0;
      else
      {
        double d = vincenty(nodes[i], nodes[j]);
        graph[i][j] = (d < THRESHOLD) ? (int)d : INF;
      }
      next[i][j] = (graph[i][j] != INF) ? j : -1;
    }
  }

  // Algorithm execution with web-friendly output
  if (choice == 1)
  { // Dijkstra
    int path[V], pathLength;
    double totalDist;
    dijkstra(srcIdx, destIdx, path, &pathLength, &totalDist);

    if (pathLength == 0 || totalDist == INF)
    {
      if (webOutput)
      {
        printErrorJSON("No valid path found");
      }
      else
      {
        printf("No valid path found.\n");
      }
      return 1;
    }

    double timeMinutes = (totalDist / WALKING_SPEED) / 60.0;
    int steps = (int)(totalDist / STEP_LENGTH);

    if (webOutput)
    {
      char pathStr[500] = "";
      for (int i = 0; i < pathLength; i++)
      {
        strcat(pathStr, nodes[path[i]].name);
        if (i < pathLength - 1)
          strcat(pathStr, " → ");
      }
      printWebJSON(choice, (int)totalDist, timeMinutes, steps, pathStr, "Dijkstra");
    }
    else
    {
      printf("\nUsing Dijkstra Algorithm...\n");
      printf("Shortest Distance: %.0f meters\n", totalDist);
      printf("Walking Time: %.2f minutes\n", timeMinutes);
      printf("Total Steps: %d\n", steps);
      printf("Path: ");
      for (int i = 0; i < pathLength; i++)
      {
        printf("%s", nodes[path[i]].name);
        if (i < pathLength - 1)
          printf(" → ");
      }
      printf("\n");
    }
  }
  else if (choice == 2)
  { // A*
    int totalDist, cameFrom[V];
    if (!aStar(graph, nodes, srcIdx, destIdx, &totalDist, cameFrom))
    {
      if (webOutput)
      {
        printErrorJSON("No valid path found");
      }
      else
      {
        printf("No valid path found.\n");
      }
      return 1;
    }

    double timeMinutes = (totalDist / WALKING_SPEED) / 60.0;
    int steps = (int)(totalDist / STEP_LENGTH);

    if (webOutput)
    {
      // Reconstruct path for A*
      char pathStr[500] = "";
      int pathNodes[V], count = 0;
      int current = destIdx;
      while (current != -1)
      {
        pathNodes[count++] = current;
        current = cameFrom[current];
      }
      for (int i = count - 1; i >= 0; i--)
      {
        strcat(pathStr, nodes[pathNodes[i]].name);
        if (i > 0)
          strcat(pathStr, " → ");
      }
      printWebJSON(choice, totalDist, timeMinutes, steps, pathStr, "A*");
    }
    else
    {
      printf("\nUsing A* Algorithm...\n");
      printf("Shortest Distance: %d meters\n", totalDist);
      printf("Walking Time: %.2f minutes\n", timeMinutes);
    }
  }
  else if (choice == 3)
  { // Floyd-Warshall
    floydWarshall(graph, next);
    int totalDist = graph[srcIdx][destIdx];

    if (totalDist == INF)
    {
      if (webOutput)
      {
        printErrorJSON("No valid path found");
      }
      else
      {
        printf("No valid path found.\n");
      }
      return 1;
    }

    double timeMinutes = (totalDist / WALKING_SPEED) / 60.0;
    int steps = (int)(totalDist / STEP_LENGTH);

    if (webOutput)
    {
      // Reconstruct path for Floyd-Warshall
      char pathStr[500] = "";
      int u = srcIdx;
      strcat(pathStr, nodes[u].name);
      while (u != destIdx)
      {
        u = next[u][destIdx];
        strcat(pathStr, " → ");
        strcat(pathStr, nodes[u].name);
      }
      printWebJSON(choice, totalDist, timeMinutes, steps, pathStr, "Floyd-Warshall");
    }
    else
    {
      printf("\nUsing Floyd-Warshall Algorithm...\n");
      printf("Shortest Distance: %d meters\n", totalDist);
      printf("Walking Time: %.2f minutes\n", timeMinutes);
    }
  }
  else if (choice == 4)
  { // Comparison
    if (webOutput)
    {
      printErrorJSON("Comparison mode not available for web interface");
    }
    else
    {
      // Your existing comparison code...
      printf("\nComparing All Algorithms...\n");
      // Keep existing comparison logic for console mode
    }
  }
  else
  {
    if (webOutput)
    {
      printErrorJSON("Invalid algorithm choice");
    }
    else
    {
      printf("Invalid choice. Please select 1, 2, 3, or 4.\n");
    }
    return 1;
  }

  return 0;
}