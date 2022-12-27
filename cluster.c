/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

#define LINE_LEN 20
/*****************************************************************
 * Declaration of necessary data types:
 */

/**
 * Struct which represents object with its ID and x and y coordinates
*/
struct obj_t {
    int id;
    float x;
    float y;
};

/**
 * Struct which represents cluster of objects
*/
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * FUNCTIONS FOR WORK WITH CLUSTERS
 */

/**
 * Initialization of cluster 'c'.
 * Function allocates memory for capacity (cap) of objects in cluster.
 * Pointer NULL at array of objects means capacity 0.
 * 
 * @param c cluster for initialization 
 * @param cap required capacity for allocation
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    c->size = 0;

	if (cap > 0) {
	
		if ((c->obj = malloc(cap * sizeof(struct obj_t)))) {
			c->capacity = cap;
			return;
		}
	}
	c->capacity = 0;
	c->obj = NULL;
}

/**
 * Function removes all objects in cluster 'c' and initalizes cluster to empty cluster.
 * 
 * @param c cluster to be cleared
*/
void clear_cluster(struct cluster_t *c)
{
    assert(c);

    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
	const int CLUSTER_CHUNK = 10;

/**
 * Function resizes cluster 'c' to new capacity (new_cap).
 * Returns cluster with new capacity.
 * In case of unsuccessfull reallocation returns NULL.
 * 
 * @param c cluster to be resized
 * @param new_cap new capacity 
*/
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/**
 * Function adds object 'obj' at the end of cluster 'c'.
 * Cluster is resized in case, that capacity of cluster 'c' is too low.
 * 
 * @param c cluster where object will be added
 * @param obj object to be added at the end of cluster
*/
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->size >= c->capacity){
        resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    }

    assert(c->obj);
    c->obj[c->size] = obj;
    c->size++;
}

/**
 * Function sorts objects in ascending order in cluster 'c' by their ID.
 * 
 * @param c cluster where objects will be sorted
*/
void sort_cluster(struct cluster_t *c);

/**
 * Function adds objects from cluster 'c2' to cluster 'c1'.
 * When capacity of cluster 'c1' is too low, cluster will be resized to new capacity.
 * All objects in cluster 'c1' are sorted ascending by their ID.
 * Cluster 'c2' stays unchanged.
 * 
 * @param c1 cluster, where objects from cluster 'c2' will be added
 * @param c2 cluster, whose objects will be added to cluster 'c1' 
*/
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

	for(int i = 0; i < c2->size; i++){
		append_cluster(c1, c2->obj[i]);
	}

	sort_cluster(c1);
}

/*********************************************************************
 * FUNCTIONS FOR WORK WITH CLUSTER ARRAY
*/

/**
 * Function removes cluster from array of clusters 'carr' at the index 'idx'.
 * Returns new number of clusters in cluster array.
 * 
 * @param carr cluster array
 * @param narr number of clusters in cluster array
 * @param idx index of cluster in cluster array to be removed
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

	int new_narr = narr - 1;
	clear_cluster(&carr[idx]);

	for(int i = idx; i < new_narr; i++){
		carr[i] = carr[i + 1];
	}

	return new_narr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
/**
 * Function calculates and returns Euclidean distance between objects 'o1' and 'o2'.
 * 
 * @param o1 first object
 * @param o2 second object
*/
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float distance = sqrtf(powf(o1->x - o2->x, 2.0) + powf(o1->y - o2->y, 2.0));
    return distance;
}

/**
 * Function calculates distance between clusters 'c1' and 'c2'.
 * Returns smallest distance between objects of cluster 'c1' and 'c2'
 * 
 * @param c1 first cluster
 * @param c2 second cluster
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

	float distance; 
    // Minimal distance is set to maximum value coordinate x or y can be
	float min_distance = 1000.0;

	for(int i = 0; i < c1->size; i++){
		for(int j = 0; j < c2->size; j++){

			distance = obj_distance(&c1->obj[i], &c2->obj[j]);
    
			if(distance < min_distance){
				min_distance = distance;
			}
		}
	}
	return min_distance;
}

/**
 * Function finds two closest clusters in cluster array 'carr'.
 * When two closest clusters are found, their indexes are saved to adresses of 'c1' and 'c2'.
 * 
 * @param carr cluster array
 * @param narr number of clusters in cluster array
 * @param c1 index of first found cluster
 * @param c2 index of second found cluster 
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

	float distance;
	float min_distance = 1000.0;
	for(int i = 0; i < narr; i++){
		for(int j = 0; j < narr; j++){
			distance = cluster_distance(&carr[i], &carr[j]);
			if(distance == 0.0){
				continue;

			}else if(distance < min_distance){
				min_distance = distance;
				*c1 = i;
				*c2 = j;
			}
		}
	}
}


/**
 * Helper function for cluster sorting.
*/
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
 * Function sorts objects in ascending order in cluster 'c' by their ID.
 * 
 * @param c cluster where objects will be sorted
*/
void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * Function prints cluster 'c' to stdout.
 * 
 * @param c cluster to be printed
*/
void print_cluster(struct cluster_t *c)
{
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*****************************************************************
 * HELPER FUNCTIONS
 */

/**
 * Function counts and returns number of lines in a file.
 * 
 * @param filename name of the file, where lines will be counted
*/
int check_lines(char *filename)
{
    char c; 
    int line_count = 0;
    FILE * file = fopen(filename, "r");
    
    for(c = getc(file); c != EOF; c = getc(file)){
        if(c == '\n') line_count++;
    }

    fclose(file);
    return line_count;
}

/**
 * Function checks if two objects in array of clusters have same ID.
 * 
 * @param carr array of clusters
 * @param size number of objects to be checked
*/
int check_id(struct cluster_t *carr, int size)
{    
    for(int i = 0; i < size - 1; i++){
        for(int j = i + 1; j < size; j++){
            if(carr[i].obj->id == carr[j].obj->id){
                return -1;
            }
        }
    }
    return 1;
}

/**
 * Function clears all existing clusters.
 * 
 * @param arr array of clusters
 * @param size number of clusters in array of clusters
*/
void free_clusters(struct cluster_t *arr, int size)
{
    for (int i = 0; i < size; i++) {
		clear_cluster(&arr[i]);
	}
	
	free(arr);
}

/*****************************************************************
 * CLUSTER LOADING AND PRINTING FUNCTIONS
 */

/**
 * Function loads objects from file 'filename'.
 * For every object, cluster is created and saved in array of clusters.
 * Returns number of loaded objects from file 'filename', in case of error, -1 is returned.
 * 
 * @param filename name of the file from which objects will be loaded
 * @param arr pointer to array of clusters
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    *arr = NULL;
    FILE * file = fopen(filename, "r");

    // Checking, if the file was opened successfully.
    if(! file){
        fprintf(stderr, "File was not opened successfully\n");
        return -1;
    }
    
    int line_len = 12;
    int object_count, id;
    int header;
    float x, y;
    char end;
    char line[line_len];
    struct cluster_t* clusters;

    fgets(line, line_len, file);

    // Checking if the first line with number of objects in file has the right format
    header = sscanf(line, "count=%d", &object_count);
    if(header == 0){
        fprintf(stderr, "Invalid format of header\n");
        fclose(file);
        return -1;
    }
    
    // Checking if number of objects declared at first line of file is not zero.
    if(object_count <= 0){
        fprintf(stderr, "Value of object count (count=%d) must be greater than 0\n", object_count);
        fclose(file);
        return -1;
    }

    // Checking if number of objects declared at first line of file is equal to actual number of objects.
    int check = check_lines(filename);
    if(check < object_count){
        fprintf(stderr, "Too little lines in file\n");
        fclose(file);
        return -1;
    }
    
    // Allocating necessary memory for array of clusters.
    clusters = malloc(sizeof(struct cluster_t) * object_count);
    
    // Checking if memory was allocated successfully.
    if(clusters == NULL){
        fprintf(stderr, "Error when allocating memory\n");
        fclose(file);
        return -1;
    }

    *arr = clusters;

    // Loading objects from file into clusters.
    for(int i = 0; i < object_count; i++){

        // Reading line and saving ID, x and y coordinates to variables.
        fgets(line, line_len, file);
        int line_format = sscanf(line, "%d %f %f%c", &id, &x, &y, &end);

        // Checking, if x or y coordinate are in valid range and if line was in correct format. 
        if(x > 1000 || x < 0 || y > 1000 || y < 0 || line_format < 3){
            fprintf(stderr, "Invalid coordinates or ID format\n");
            free_clusters(*arr, i);
            fclose(file);
            return -1;
        
        // Checking, that two objects are not on same line.
        }else if(end != '\n'){
            fprintf(stderr, "Two objects are on same line\n");
            free_clusters(*arr, i);
            fclose(file);
            return -1;
        }

        // Checking if ID, x and y coordinates are whole numbers.
        if(id - id != 0 || x - (int)x != 0 || y - (int)y != 0){
            fprintf(stderr, "ID, x or y is not a whole number\n");
            free_clusters(*arr, i);
            fclose(file);
            return -1;
        }

        struct obj_t obj;
        struct cluster_t *cluster;

        // Declaring, that pointer to cluster 'cluster' points to right index in array of clusters.
        cluster = clusters + i;

        // Initialization of cluster
        init_cluster((cluster),1);

        // Saving values of ID, x and y coordinates in object.
        obj.id = id;
        obj.x = x;
        obj.y = y;

        // Adding cluster with one object into array of clusters.
        append_cluster(cluster, obj);
    }

    fclose(file);
    return object_count;
}

/**
 * Printing all clusters from array of clusters 'carr' to stdout.
 * 
 * @param carr array of clusters to be printed
 * @param narr number of clusters in array of clusters
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*****************************************************************
 * MAIN FUNCTION AND ARGUMENT PROCESSING
 */

/**
 * Main function.
 * 
 * @param argc number of arguments
 * @param argv arguments
*/
int main(int argc,char *argv[])
{
    float toPrint;

    // Checking if at least argument with name of the file is inserted.
    if(argc == 1){
        fprintf(stderr, "At least one argument (with the name of a file) must be inserted\n");
        return -1;

    // If no number of clusters to be printed is inserted, 'toPrint' is set to 1.
    }else if(argc == 2){
        toPrint = 1;
    
    // Second argument (number of clusters to be printed) format validation.
    }else if(argc == 3){
        toPrint = atof(argv[2]);

        if(toPrint == 0){
            fprintf(stderr, "Second argument must be a number\n");
            return -1;
        }
        else if(toPrint - (int)toPrint != 0){
            fprintf(stderr, "Second argument must me a whole number\n");
            return -1;
        }
    }
    // Checking that no more than 3 arguments (name of program, file name and number of clusters to be printed) are inserted.
        if(argc > 3){
        fprintf(stderr, "Too many arguments inserted\n");
        return -1;
    }
    
    struct cluster_t *clusters;
    
	int idx1 = 0;
    int idx2 = 0;

    // Loading clusters and saving their count into 'loaded'.
    int loaded = load_clusters(argv[1], &clusters);
    
    // Checking that no objects have same ID.
    if(check_id(clusters, loaded) == -1){
        fprintf(stderr, "Two objects have identical ID\n");
        free_clusters(clusters, loaded);
        return -1;
    }
    
    if(loaded == -1){
        return -1;
    }
    // Checking that desired number of clusters to be printed is not higher, than number of loaded clusters.
    else if(toPrint > loaded){
        fprintf(stderr, "Number of clusters to be printed is greater than total number of objects\n");
        free_clusters(clusters, loaded);
        return -1;
    }
    // Creating desired number of clusters to be printed.
    while(loaded > toPrint)
    {
    find_neighbours(clusters, loaded, &idx1, &idx2);
    merge_clusters(&clusters[idx1], &clusters[idx2]);
    loaded = remove_cluster(clusters, loaded, idx2);
    }

    print_clusters(clusters, toPrint);
	free_clusters(clusters, loaded);
    return 0;
}