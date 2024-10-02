#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 50000 // Tamanho da HashTable.

unsigned long hash_function(char *str){
    unsigned long i = 0;

    for (int j = 0; str[j]; j++)
        i += str[j];

    return i % CAPACITY;
}

// Define o item da HashTable.
typedef struct Ht_item{
    char *key;
    char *value;
} Ht_item;

// Define a LinkedList.
typedef struct LinkedList{
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;

// Define a HashTable.
typedef struct HashTable{
    // Contém um array de ponteiros para os itens.
    Ht_item **items;
    LinkedList **overflow_buckets;
    int size;
    int count;
} HashTable;

LinkedList *allocate_list(){
    // Aloca memória para um ponteiro LinkedList.
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    return list;
}

LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item){
    // Insere o item na LinkedList.
    if (!list)
    {
        LinkedList *head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }
    else if (list->next == NULL)
    {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;

    while (temp->next->next)
    {
        temp = temp->next;
    }

    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    return list;
}

Ht_item *linkedlist_remove(LinkedList *list){
    // Remove o primeiro elemento da LinkedList.
    // Retorna o item do elemento removido.
    if (!list)
        return NULL;

    if (!list->next)
        return NULL;

    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    Ht_item *it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item));
    free(temp->item->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}

void free_linkedlist(LinkedList *list){
    LinkedList *temp = list;

    while (list)
    {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}

LinkedList **create_overflow_buckets(HashTable *table){
    // Cria os buckets de overflow; um array de LinkedLists.
    LinkedList **buckets = (LinkedList **)calloc(table->size, sizeof(LinkedList *));

    for (int i = 0; i < table->size; i++)
        buckets[i] = NULL;

    return buckets;
}

void free_overflow_buckets(HashTable *table){
    // Libera todas as listas de buckets de overflow.
    LinkedList **buckets = table->overflow_buckets;

    for (int i = 0; i < table->size; i++)
        free_linkedlist(buckets[i]);

    free(buckets);
}

Ht_item *create_item(char *key, char *value){
    // Cria um ponteiro para um novo item da HashTable.
    Ht_item *item = (Ht_item *)malloc(sizeof(Ht_item));
    item->key = (char *)malloc(strlen(key) + 1);
    item->value = (char *)malloc(strlen(value) + 1);
    strcpy(item->key, key);
    strcpy(item->value, value);
    return item;
}

HashTable *create_table(int size){
    // Cria uma nova HashTable.
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item **)calloc(table->size, sizeof(Ht_item *));

    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;

    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

void free_item(Ht_item *item){
    // Libera um item.
    free(item->key);
    free(item->value);
    free(item);
}

void free_table(HashTable *table){
    // Libera a tabela.
    for (int i = 0; i < table->size; i++)
    {
        Ht_item *item = table->items[i];

        if (item != NULL)
            free_item(item);
    }

    // Libera as listas de buckets de overflow e seus itens.
    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision(HashTable *table, unsigned long index, Ht_item *item){
    LinkedList *head = table->overflow_buckets[index];

    if (head == NULL)
    {
        // Cria a lista.
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else
    {
        // Insere na lista.
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

void ht_insert(HashTable *table, char *key, char *value){
    // Cria o item.
    Ht_item *item = create_item(key, value);

    // Calcula o índice.
    int index = hash_function(key);

    Ht_item *current_item = table->items[index];

    if (current_item == NULL)
    {
        // A chave não existe.
        if (table->count == table->size)
        {
            // A HashTable está cheia.
            printf("Erro de Inserção: HashTable está cheia\n");
            free_item(item);
            return;
        }

        // Insere diretamente.
        table->items[index] = item;
        table->count++;
    }
    else
    {
        // Cenário 1: Atualiza o valor.
        if (strcmp(current_item->key, key) == 0)
        {
            strcpy(table->items[index]->value, value);
            return;
        }
        else
        {
            // Cenário 2: Trata a colisão.
            handle_collision(table, index, item);
            return;
        }
    }
}

char *ht_search(HashTable *table, char *key){
    // Procura a chave na HashTable.
    // Retorna NULL se não existir.
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->overflow_buckets[index];

    // Fornece apenas valores não NULL.
    if (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
            return item->value;

        if (head == NULL)
            return NULL;

        item = head->item;
        head = head->next;
    }

    return NULL;
}

void ht_delete(HashTable *table, char *key){
    // Deleta um item da tabela.
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->overflow_buckets[index];

    if (item == NULL)
    {
        // Não existe.
        return;
    }
    else
    {
        if (head == NULL && strcmp(item->key, key) == 0)
        {
            // A cadeia de colisão não existe.
            // Remove o item.
            // Define o índice da tabela como NULL.
            table->items[index] = NULL;
            free_item(item);
            table->count--;
            return;
        }
        else if (head != NULL)
        {
            // A cadeia de colisão existe.
            if (strcmp(item->key, key) == 0)
            {
                // Remove este item.
                // Define o início da lista como o novo item.
                free_item(item);
                LinkedList *node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item(node->item->key, node->item->value);
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            LinkedList *curr = head;
            LinkedList *prev = NULL;

            while (curr)
            {
                if (strcmp(curr->item->key, key) == 0)
                {
                    if (prev == NULL)
                    {
                        // Primeiro elemento da cadeia.
                        // Remove a cadeia.
                        free_linkedlist(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else
                    {
                        // Este está em algum lugar da cadeia.
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }

                curr = curr->next;
                prev = curr;
            }
        }
    }
}

void print_search(HashTable *table, char *key){
    char *val;

    if ((val = ht_search(table, key)) == NULL)
    {
        printf("Chave:%s não existe\n", key);
        return;
    }
    else
    {
        printf("Chave:%s, Valor:%s\n", key, val);
    }
}

void print_table(HashTable *table){
    printf("\nHash Table\n-------------------\n");

    for (int i = 0; i < table -> size; i++)
    {
        if (table -> items[i])
        {
            printf("Índice:%d, Chave:%s, Valor:%s\n", i, table -> items[i] -> key, table -> items[i] -> value);
        }
    }

    printf("-------------------\n\n");
}
