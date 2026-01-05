#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
    char data;
    struct TreeNode *lchild;
    struct TreeNode *rchild;
} TreeNode;

/* ===================== 建树：前序 + # 为空 ===================== */
TreeNode* buildFromPreorder(const char *pre, int *idx) {
    char c = pre[*idx];
    if (c == '\0') return NULL;
    (*idx)++;

    if (c == '#') return NULL;

    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = c;
    node->lchild = buildFromPreorder(pre, idx);
    node->rchild = buildFromPreorder(pre, idx);
    return node;
}

/* ===================== 树高：递归 ===================== */
int treeHeight(TreeNode *root) {
    if (!root) return 0;
    int hl = treeHeight(root->lchild);
    int hr = treeHeight(root->rchild);
    return (hl > hr ? hl : hr) + 1;
}

/* ===================== 后序遍历：递归 ===================== */
void postorder(TreeNode *root, char *result, int *pos) {
    if (!root) return;
    postorder(root->lchild, result, pos);
    postorder(root->rchild, result, pos);
    result[(*pos)++] = root->data;
}

/* ===================== 根到任意结点路径：DFS ===================== */
int dfsFindPath(TreeNode *root, char target, char path[], int depth, char *result) {
    if (!root) return 0;

    path[depth] = root->data;

    if (root->data == target) {
        // 生成路径字符串
        int i;
        for (i = 0; i <= depth; i++) {
            if (i > 0) {
                strcat(result, "->");
            }
            char buf[2] = {path[i], '\0'};
            strcat(result, buf);
        }
        return 1;
    }

    if (dfsFindPath(root->lchild, target, path, depth + 1, result)) return 1;
    if (dfsFindPath(root->rchild, target, path, depth + 1, result)) return 1;

    return 0;
}

/* ===================== 结点所处高度：BFS（层序） ===================== */
typedef struct QNode {
    TreeNode *node;
    int level;
} QNode;

typedef struct Queue {
    QNode *arr;
    int front, rear, cap;
} Queue;

Queue* createQueue(int cap) {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->arr = (QNode*)malloc(sizeof(QNode) * cap);
    q->front = 0;
    q->rear = 0;
    q->cap = cap;
    return q;
}

int isEmpty(Queue *q) {
    return q->front == q->rear;
}

void enqueue(Queue *q, TreeNode *node, int level) {
    if (q->rear >= q->cap) {
        q->cap *= 2;
        q->arr = (QNode*)realloc(q->arr, sizeof(QNode) * q->cap);
    }
    q->arr[q->rear].node = node;
    q->arr[q->rear].level = level;
    q->rear++;
}

QNode dequeue(Queue *q) {
    return q->arr[q->front++];
}

int bfsGetNodeLevel(TreeNode *root, char target) {
    if (!root) return -1;

    Queue *q = createQueue(64);
    enqueue(q, root, 1);

    while (!isEmpty(q)) {
        QNode cur = dequeue(q);
        TreeNode *p = cur.node;

        if (p->data == target) {
            free(q->arr);
            free(q);
            return cur.level;
        }

        if (p->lchild) enqueue(q, p->lchild, cur.level + 1);
        if (p->rchild) enqueue(q, p->rchild, cur.level + 1);
    }

    free(q->arr);
    free(q);
    return -1;
}

/* ===================== 释放树 ===================== */
void freeTree(TreeNode *root) {
    if (!root) return;
    freeTree(root->lchild);
    freeTree(root->rchild);
    free(root);
}

/* ===================== 命令行接口 ===================== */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [arguments]\n", argv[0]);
        printf("Commands:\n");
        printf("  build <preorder> - Build tree from preorder sequence\n");
        printf("  height <tree_file> - Get tree height\n");
        printf("  postorder <tree_file> - Get postorder traversal\n");
        printf("  path <tree_file> <target> - Get path from root to target\n");
        printf("  nodelevel <tree_file> <target> - Get node level\n");
        return 1;
    }

    if (strcmp(argv[1], "build") == 0) {
        if (argc < 3) {
            printf("Error: Missing preorder sequence\n");
            return 1;
        }
        
        int idx = 0;
        TreeNode *root = buildFromPreorder(argv[2], &idx);
        
        // Save tree to a temporary file for later use
        FILE *fp = fopen("tree_data.tmp", "wb");
        if (!fp) {
            printf("Error: Cannot create tree file\n");
            freeTree(root);
            return 1;
        }
        
        // We'll just save the preorder sequence for simplicity
        fwrite(argv[2], sizeof(char), strlen(argv[2]) + 1, fp);
        fclose(fp);
        
        printf("Tree built successfully.\n");
        printf("Preorder: %s\n", argv[2]);
        freeTree(root);
    }
    else if (strcmp(argv[1], "height") == 0) {
        if (argc < 3) {
            printf("Error: Missing tree file\n");
            return 1;
        }
        
        FILE *fp = fopen(argv[2], "rb");
        if (!fp) {
            printf("Error: Cannot open tree file\n");
            return 1;
        }
        
        char preorder[2048];
        fread(preorder, sizeof(char), 2048, fp);
        fclose(fp);
        
        int idx = 0;
        TreeNode *root = buildFromPreorder(preorder, &idx);
        int height = treeHeight(root);
        printf("%d\n", height);
        freeTree(root);
    }
    else if (strcmp(argv[1], "postorder") == 0) {
        if (argc < 3) {
            printf("Error: Missing tree file\n");
            return 1;
        }
        
        FILE *fp = fopen(argv[2], "rb");
        if (!fp) {
            printf("Error: Cannot open tree file\n");
            return 1;
        }
        
        char preorder[2048];
        fread(preorder, sizeof(char), 2048, fp);
        fclose(fp);
        
        int idx = 0;
        TreeNode *root = buildFromPreorder(preorder, &idx);
        char result[2048] = {0};
        int pos = 0;
        postorder(root, result, &pos);
        printf("%s\n", result);
        freeTree(root);
    }
    else if (strcmp(argv[1], "path") == 0) {
        if (argc < 4) {
            printf("Error: Missing tree file or target\n");
            return 1;
        }
        
        FILE *fp = fopen(argv[2], "rb");
        if (!fp) {
            printf("Error: Cannot open tree file\n");
            return 1;
        }
        
        char preorder[2048];
        fread(preorder, sizeof(char), 2048, fp);
        fclose(fp);
        
        char target = argv[3][0];
        int idx = 0;
        TreeNode *root = buildFromPreorder(preorder, &idx);
        char path[2048] = {0};
        char result[2048] = {0};
        
        if (dfsFindPath(root, target, path, 0, result)) {
            printf("%s\n", result);
        } else {
            printf("-1\n");
        }
        freeTree(root);
    }
    else if (strcmp(argv[1], "nodelevel") == 0) {
        if (argc < 4) {
            printf("Error: Missing tree file or target\n");
            return 1;
        }
        
        FILE *fp = fopen(argv[2], "rb");
        if (!fp) {
            printf("Error: Cannot open tree file\n");
            return 1;
        }
        
        char preorder[2048];
        fread(preorder, sizeof(char), 2048, fp);
        fclose(fp);
        
        char target = argv[3][0];
        int idx = 0;
        TreeNode *root = buildFromPreorder(preorder, &idx);
        int level = bfsGetNodeLevel(root, target);
        printf("%d\n", level);
        freeTree(root);
    }
    else {
        printf("Error: Unknown command '%s'\n", argv[1]);
        return 1;
    }
    
    return 0;
}