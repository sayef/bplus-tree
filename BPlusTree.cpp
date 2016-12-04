#include<bits/stdc++.h>
using namespace std;

#define MAX 50


//number of pointers or number of child blocks [numberOfPointers = numberOfNodes + 1]
int numberOfPointers;

struct Block{
    //number of nodes
    int tNodes;
    //for parent Block and index
    Block *parentBlock;
    //values
    int value[MAX];
    //child Blocks
    Block *childBlock[MAX];
    Block(){ //constructor to initialize a block
        tNodes = 0;
        parentBlock = NULL;
        for(int i=0; i<MAX; i++){
            value[i] = INT_MAX;
            childBlock[i] = NULL;
        }
    }
};
//declare root Block
Block *rootBlock = new Block();

//function to split the leaf nodes
void splitLeaf(Block *curBlock){
    int x, i, j;

    //split the greater half to the left when numberOfPointer is odd
    //else split equal equal when numberOfPointer is even
    if(numberOfPointers%2)
        x = (numberOfPointers+1)/2;
    else x = numberOfPointers/2;

    //we don't declare another block for leftBlock, rather re-use curBlock as leftBlock and
    //take away the right half values to the rightBlock
    Block *rightBlock = new Block();

    //so leftBlock has x number of nodes
    curBlock->tNodes = x;
    //and rightBlock has numberOfPointers-x
    rightBlock->tNodes = numberOfPointers-x;
    //so both of them have their common parent [even parent may be null, so both of them will have null parent]
    rightBlock->parentBlock = curBlock->parentBlock;

    for(i=x, j=0; i<numberOfPointers; i++, j++){
        //take the right-half values from curBlocks and put in the rightBlock
        rightBlock->value[j] = curBlock->value[i];
        //and erase right-half values from curBlock to make it real leftBlock
        //so that it does not contain all values only contains left-half values
        curBlock->value[i] = INT_MAX;
    }
    //for splitting the leaf blocks we copy the first item from the rightBlock to their parentBlock
    //and val contains that value
    int val = rightBlock->value[0];

    //if the leaf itself is a parent then
    if(curBlock->parentBlock==NULL){
        //it has null parent, so create a new parent
        Block *parentBlock = new Block();
        //and new parent should have a null parent
        parentBlock->parentBlock = NULL;
        //new parent will have only one member
        parentBlock->tNodes=1;
        //and that member is val
        parentBlock->value[0] = val;
        //so the parent has two child, so assign them (don't forget curBlock is actually the leftBlock)
        parentBlock->childBlock[0] = curBlock;
        parentBlock->childBlock[1] = rightBlock;
        //their parent of the left and right blocks is no longer null, so assign their parent
        curBlock->parentBlock = rightBlock->parentBlock = parentBlock;
        //from now on this parentBlock is the rootBlock
        rootBlock = parentBlock;
        return;
    }
    else{   //if the splitted leaf block is not rootBlock then

        // we have to put the val and assign the rightBlock to the right place in the parentBlock
        // so we go to the parentBlock and from now we consider the curBlock as the parentBlock of the splitted Block

        curBlock = curBlock->parentBlock;

        //for the sake of insertNodeion sort to put the rightBlock and val in the exact position
        //of th parentBlock [here curBlock] take a new child block and assign rightBlock to it
        Block *newChildBlock = new Block();
        newChildBlock = rightBlock;

        //simple insertion sort to put val at the exact position of values[] in the parentBlock [here curBlock]

        for(i=0; i<=curBlock->tNodes; i++){
            if(val < curBlock->value[i]){
                swap(curBlock->value[i], val);
            }
        }

        //after putting val number of nodes gets increase by one
        curBlock->tNodes++;

         //simple insertNodeion sort to put rightBlock at the exact position
         //of childBlock[] in the parentBlock [here curBlock]

        for(i=0; i<curBlock->tNodes; i++){
            if(newChildBlock->value[0] < curBlock->childBlock[i]->value[0]){
                swap(curBlock->childBlock[i], newChildBlock);
            }
        }
        curBlock->childBlock[i] = newChildBlock;

        //we reordered some blocks and pointers, so for the sake of safety
        //all childBlocks' should have their parent updated
        for(i=0;curBlock->childBlock[i]!=NULL;i++){
            curBlock->childBlock[i]->parentBlock = curBlock;
        }
    }

}

//function to split the non leaf nodes
void splitNonLeaf(Block *curBlock){
    int x, i, j;

    //split the less half to the left when numberOfPointer is odd
    //else split equal equal when numberOfPointer is even.  n/2 does it nicely for us

    x = numberOfPointers/2;

    //declare rightBlock and we will use curBlock as the leftBlock
    Block *rightBlock = new Block();

    //so leftBlock has x number of nodes
    curBlock->tNodes = x;
    //rightBlock has numberOfPointers-x-1 children, because we won't copy and paste
    //rather delete and paste the first item of the rightBlock
    rightBlock->tNodes = numberOfPointers-x-1;
    //both children have their common parent
    rightBlock->parentBlock = curBlock->parentBlock;


    for(i=x, j=0; i<=numberOfPointers; i++, j++){
        //copy the right-half members to the rightBlock
        rightBlock->value[j] = curBlock->value[i];
        //and also copy their children
        rightBlock->childBlock[j] = curBlock->childBlock[i];
        //erase the right-half values from curBlock to make it perfect leftBlock
        //which won't contain only left-half values and their children
        curBlock->value[i] = INT_MAX;
        //erase all the right-half childBlocks from curBlock except the x one
        //because if left child has 3 nodes then it should have 4 childBlocks, so don't delete that child
        if(i!=x)curBlock->childBlock[i] = NULL;
    }

    //we will take a copy of the first item of the rightBlock
    //as we will delete that item later from the list
    int val = rightBlock->value[0];
    //just right-shift value[] and childBlock[] by one from rightBlock
    //to have no repeat of the first item for non-leaf Block
    memcpy(&rightBlock->value, &rightBlock->value[1], sizeof(int)*(rightBlock->tNodes+1));
    memcpy(&rightBlock->childBlock, &rightBlock->childBlock[1], sizeof(rootBlock)*(rightBlock->tNodes+1));

    //we reordered some values and positions so don't forget
    //to assign the children's exact parent

    for(i=0;curBlock->childBlock[i]!=NULL;i++){
        curBlock->childBlock[i]->parentBlock = curBlock;
    }
    for(i=0;rightBlock->childBlock[i]!=NULL;i++){
        rightBlock->childBlock[i]->parentBlock = rightBlock;
    }

    //if the splitted block itself a parent
    if(curBlock->parentBlock==NULL){
        //create a new parent
        Block *parentBlock = new Block();
        //parent should have a null parent
        parentBlock->parentBlock = NULL;
        //parent will have only one node
        parentBlock->tNodes=1;
        //the only value is the val
        parentBlock->value[0] = val;
        //it has two children, leftBlock and rightBlock
        parentBlock->childBlock[0] = curBlock;
        parentBlock->childBlock[1] = rightBlock;

        //and both rightBlock and leftBlock has no longer null parent, they have their new parent
        curBlock->parentBlock = rightBlock->parentBlock = parentBlock;

        //from now on this new parent is the root parent
        rootBlock = parentBlock;
        return;
    }
    else{   //if the splitted leaf block is not rootBlock then

        // we have to put the val and assign the rightBlock to the right place in the parentBlock
        // so we go to the parentBlock and from now we consider the curBlock as the parentBlock of the splitted Block
        curBlock = curBlock->parentBlock;

        //for the sake of insertNodeion sort to put the rightBlock and val in the exact position
        //of th parentBlock [here curBlock] take a new child block and assign rightBlock to it

        Block *newChildBlock = new Block();
        newChildBlock = rightBlock;

        //simple insertion sort to put val at the exact position of values[] in the parentBlock [here curBlock]


        for(i=0; i<=curBlock->tNodes; i++){
            if(val < curBlock->value[i]){
                swap(curBlock->value[i], val);
            }
        }

        //after putting val number of nodes gets increase by one
        curBlock->tNodes++;

        //simple insertNodeion sort to put rightBlock at the exact position
         //of childBlock[] in the parentBlock [here curBlock]

        for(i=0; i<curBlock->tNodes; i++){
            if(newChildBlock->value[0] < curBlock->childBlock[i]->value[0]){
                swap(curBlock->childBlock[i], newChildBlock);
            }
        }
        curBlock->childBlock[i] = newChildBlock;

        //we reordered some blocks and pointers, so for the sake of safety
        //all childBlocks' should have their parent updated
         for(i=0;curBlock->childBlock[i]!=NULL;i++){
            curBlock->childBlock[i]->parentBlock = curBlock;
        }
    }

}

void insertNode(Block *curBlock, int val){

    for(int i=0; i<=curBlock->tNodes; i++){
        if(val < curBlock->value[i] && curBlock->childBlock[i]!=NULL){
            insertNode(curBlock->childBlock[i], val);
            if(curBlock->tNodes==numberOfPointers)
                splitNonLeaf(curBlock);
            return;
        }
        else if(val < curBlock->value[i] && curBlock->childBlock[i]==NULL){
            swap(curBlock->value[i], val);
            //swap(curBlock->childBlock[i], newChildBlock);
            if(i==curBlock->tNodes){
                    curBlock->tNodes++;
                    break;
            }
        }
    }

    if(curBlock->tNodes==numberOfPointers){

            splitLeaf(curBlock);
    }
}


void redistributeBlock(Block *leftBlock, Block *rightBlock, bool isLeaf, int posOfLeftBlock, int whichOneisCurBlock){

    //re-distribution will affect the first value of the rightBlock, so remember it
    //for later replacement of the copy of this value somewhere in ancestor Block
    int PrevRightFirstVal = rightBlock->value[0];


    if(whichOneisCurBlock==0){ //leftBlock is curBlock

        //if the blocks are not leaf node
        if(!isLeaf){
            //bring down the value from which it is left child in parentBlock
            leftBlock->value[leftBlock->tNodes] = leftBlock->parentBlock->value[posOfLeftBlock];
            //the right child of newly added value should be the left child of first value of rightBlock
            leftBlock->childBlock[leftBlock->tNodes+1] = rightBlock->childBlock[0];
            //increase leftBlock's number of nodes by one
            leftBlock->tNodes++;
            //send up a the first value of the rightBlock to the parentBlock
            leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
            //shift left by one in rightBlock
            memcpy(&rightBlock->value[0], &rightBlock->value[1], sizeof(int)*(rightBlock->tNodes+1));
            memcpy(&rightBlock->childBlock[0], &rightBlock->childBlock[1], sizeof(rootBlock)*(rightBlock->tNodes+1));
            rightBlock->tNodes--;

        }else{
            //borrow the first value of rightBlock to the last position of leftBlock
            leftBlock->value[leftBlock->tNodes] = rightBlock->value[0];
            leftBlock->tNodes++;
            //shift by one node to left of the rightBlock
            memcpy(&rightBlock->value[0], &rightBlock->value[1], sizeof(int)*(rightBlock->tNodes+1));
            //decrease number of nodes by one
            rightBlock->tNodes--;

            leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
        }



    }else{ //rightBlock is curBlock

        if(!isLeaf){

            //shift right by one in rightBlock so that first position becomes free
            memcpy(&rightBlock->value[1], &rightBlock->value[0], sizeof(int)*(rightBlock->tNodes+1));
            memcpy(&rightBlock->childBlock[1], &rightBlock->childBlock[0], sizeof(rootBlock)*(rightBlock->tNodes+1));
            //bring down the value from which it is left child in parentBlock to first pos of rightBlock
            rightBlock->value[0] = leftBlock->parentBlock->value[posOfLeftBlock];
            //and the left child of the newly first value of right child will be the last child of leftBlock
            rightBlock->childBlock[0] = leftBlock->childBlock[leftBlock->tNodes];

            rightBlock->tNodes++;

            //send up a the last value of the leftBlock to the parentBlock
            leftBlock->parentBlock->value[posOfLeftBlock] = leftBlock->value[leftBlock->tNodes-1];
            //erase the last element and pointer of leftBlock
            leftBlock->value[leftBlock->tNodes-1] = INT_MAX;
            leftBlock->childBlock[leftBlock->tNodes] = NULL;
            leftBlock->tNodes--;

        }else{

            //shift by one node to right of the rightBlock so that we can free the first position
            memcpy(&rightBlock->value[1], &rightBlock->value[0], sizeof(int)*(rightBlock->tNodes+1));
            //borrow the last value of leftBlock to the first position of rightBlock
            rightBlock->value[0] = leftBlock->value[leftBlock->tNodes-1];
            //increase number of nodes by one
            rightBlock->tNodes++;

            leftBlock->value[leftBlock->tNodes-1] = INT_MAX;
            leftBlock->tNodes--;

            leftBlock->parentBlock->value[posOfLeftBlock] = rightBlock->value[0];
        }
    }
}

void mergeBlock(Block *leftBlock, Block *rightBlock, bool isLeaf, int posOfRightBlock){

    //cout << "leftBlock " << leftBlock->value[0] << " rightBlock " << rightBlock->value[0] << endl;
    //cout << "size " << leftBlock->tNodes << " size " << rightBlock->tNodes << endl;
    if(!isLeaf){

        leftBlock->value[leftBlock->tNodes] = leftBlock->parentBlock->value[posOfRightBlock-1];
        leftBlock->tNodes++;
    }

    memcpy(&leftBlock->value[leftBlock->tNodes], &rightBlock->value[0], sizeof(int)*(rightBlock->tNodes+1));
    memcpy(&leftBlock->childBlock[leftBlock->tNodes], &rightBlock->childBlock[0], sizeof(rootBlock)*(rightBlock->tNodes+1));


    leftBlock->tNodes += rightBlock->tNodes;


    //cout << "before: " << leftBlock->parentBlock->value[1] << endl;
    memcpy(&leftBlock->parentBlock->value[posOfRightBlock-1], &leftBlock->parentBlock->value[posOfRightBlock], sizeof(int)*(leftBlock->parentBlock->tNodes+1));
    memcpy(&leftBlock->parentBlock->childBlock[posOfRightBlock], &leftBlock->parentBlock->childBlock[posOfRightBlock+1], sizeof(rootBlock)*(leftBlock->parentBlock->tNodes+1));
    //cout << "after merging " << leftBlock->parentBlock->childBlock[posOfRightBlock-2]->value[0] << " and ";// << leftBlock->parentBlock->childBlock[posOfRightBlock]->value[0] << endl;
    leftBlock->parentBlock->tNodes--;

    //we reordered some blocks and pointers, so for the sake of safety
    //all childBlocks' should have their parent updated
     for(int i=0;leftBlock->childBlock[i]!=NULL;i++){
        leftBlock->childBlock[i]->parentBlock = leftBlock;
    }



}
bool dataFound;
void deleteNode(Block *curBlock, int val, int curBlockPosition){

     //to check if the current block is a leaf or not
     bool isLeaf;
     if(curBlock->childBlock[0]==NULL)
        isLeaf = true;
     else isLeaf = false;

    //left most value could be changed due to merge or re-distribution later,
    //so keep it to replace it's copy from it's ancestor
    int prevLeftMostVal = curBlock->value[0];



     for(int i=0;dataFound==false &&  i<=curBlock->tNodes; i++){
        if(val < curBlock->value[i] && curBlock->childBlock[i] != NULL){
            deleteNode(curBlock->childBlock[i], val, i);

        }
        //if we could find the target value at any leafBlock then
        else if(val == curBlock->value[i] && curBlock->childBlock[i] == NULL){

            //delete the node by shifting all values and pointers  by one to the left
            memcpy(&curBlock->value[i], &curBlock->value[i+1], sizeof(int)*(curBlock->tNodes+1));
            //decrease number of nodes by one
            curBlock->tNodes--;
            dataFound = true;
            break;
        }
     }

    //if the root is the only leaf
     if(curBlock->parentBlock == NULL && curBlock->childBlock[0] == NULL){
        return;
     }


  //if the curBlock is rootBlock and it has one pointers only
    if(curBlock->parentBlock==NULL && curBlock->childBlock[0] != NULL && curBlock->tNodes == 0){
        rootBlock = curBlock->childBlock[0];
        rootBlock->parentBlock = NULL;
        return;
    }


    //now check if the curBlock has less than half of the number of maximum node
    //cout << curBlockPosition << endl;
    //if less than half we will try to re-distribute first

    //cout << curBlock->childBlock[0]->value[0] << " "<< curBlockPosition << endl;
    if(isLeaf && curBlock->parentBlock!=NULL){

        if(curBlockPosition==0){
            Block *rightBlock = new Block();
            rightBlock = curBlock->parentBlock->childBlock[1];

            //if we the right one has more than half nodes of maximum capacity than re-distribute
            if(rightBlock!=NULL && rightBlock->tNodes > (numberOfPointers+1)/2){

                    redistributeBlock(curBlock, rightBlock, isLeaf, 0, 0);
            }
            //else there is nothing to re-distribute, so we can merge them
            else if (rightBlock!=NULL && curBlock->tNodes+rightBlock->tNodes < numberOfPointers){

                    mergeBlock(curBlock, rightBlock, isLeaf, 1);
            }
        }

        else{


            Block *leftBlock = new Block();
            Block *rightBlock = new Block();


            leftBlock = curBlock->parentBlock->childBlock[curBlockPosition-1];

            rightBlock = curBlock->parentBlock->childBlock[curBlockPosition+1];


            //if we see that left one has more than half nodes of maximum capacity then try to re-distribute
            if(leftBlock!=NULL && leftBlock->tNodes > (numberOfPointers+1)/2){
                 redistributeBlock(leftBlock, curBlock, isLeaf, curBlockPosition-1, 1);
            }
            else if(rightBlock!=NULL && rightBlock->tNodes > (numberOfPointers+1)/2){
                redistributeBlock(curBlock, rightBlock, isLeaf, curBlockPosition, 0);
            }
            else if (leftBlock!=NULL && curBlock->tNodes+leftBlock->tNodes < numberOfPointers){
                mergeBlock(leftBlock, curBlock, isLeaf, curBlockPosition);
            }
            else if (rightBlock!=NULL && curBlock->tNodes+rightBlock->tNodes < numberOfPointers){
               mergeBlock(curBlock, rightBlock, isLeaf, curBlockPosition+1);
            }
        }
    }
    else if(!isLeaf && curBlock->parentBlock!=NULL){

        if(curBlockPosition==0){
            Block *rightBlock = new Block();
            rightBlock = curBlock->parentBlock->childBlock[1];

            //if we see the right one has more than half nodes of maximum capacity than re-distribute
            if( rightBlock!=NULL && rightBlock->tNodes-1 >= ceil((numberOfPointers-1)/2) ){
                redistributeBlock(curBlock, rightBlock, isLeaf, 0, 0);
            }
            //else there is nothing to re-distribute, so we can merge them
            else if (rightBlock!=NULL && curBlock->tNodes+rightBlock->tNodes < numberOfPointers - 1){
                mergeBlock(curBlock, rightBlock, isLeaf, 1);
            }
        }
        //for any other case we can safely take the left one to try for re-distribution
        else{


            Block *leftBlock = new Block();
            Block *rightBlock = new Block();


            leftBlock = curBlock->parentBlock->childBlock[curBlockPosition-1];

            rightBlock = curBlock->parentBlock->childBlock[curBlockPosition+1];


            //if we see that left one has more than half nodes of maximum capacity then try to re-distribute
            if( leftBlock!=NULL && leftBlock->tNodes-1 >= ceil((numberOfPointers-1)/2)){
                 redistributeBlock(leftBlock, curBlock, isLeaf, curBlockPosition-1, 1);
            }
            else if(rightBlock!=NULL && rightBlock->tNodes-1 >=  ceil((numberOfPointers-1)/2)){
                redistributeBlock(curBlock, rightBlock, isLeaf, curBlockPosition, 0);
            }
            //else there is nothing to re-distribute, so we merge them

            else if ( leftBlock!=NULL && curBlock->tNodes+leftBlock->tNodes < numberOfPointers-1) {
                mergeBlock(leftBlock, curBlock, isLeaf, curBlockPosition);
            }
            else if ( rightBlock!=NULL && curBlock->tNodes+rightBlock->tNodes < numberOfPointers-1){
               mergeBlock(curBlock, rightBlock, isLeaf, curBlockPosition+1);
            }
        }

    }



    //delete the duplicate if any in the ancestor Block
    Block *tempBlock = new Block();
    tempBlock = curBlock->parentBlock;
    while(tempBlock!=NULL){
            for(int i=0; i<tempBlock->tNodes;i++){
                if(tempBlock->value[i]==prevLeftMostVal){
                    tempBlock->value[i] = curBlock->value[0];
                    break;
                }
        }
        tempBlock = tempBlock->parentBlock;
    }

}

void print(vector < Block* > Blocks){
    vector < Block* > newBlocks;
    for(int i=0; i<Blocks.size(); i++){ //for every block
        Block *curBlock = Blocks[i];

        cout <<"[|";
        int j;
        for(j=0; j<curBlock->tNodes; j++){  //traverse the childBlocks, print values and save all the childBlocks
            cout << curBlock->value[j] << "|";
            if(curBlock->childBlock[j]!=NULL)
            newBlocks.push_back(curBlock->childBlock[j]);
        }
        if(curBlock->value[j]==INT_MAX && curBlock->childBlock[j]!=NULL)
            newBlocks.push_back(curBlock->childBlock[j]);

        cout << "]  ";
    }

    if(newBlocks.size()==0){ //if there is no childBlock block left to send out then just the end of the recursion

        puts("");
        puts("");
        Blocks.clear();
        //exit(0);
    }
    else {                    //else send the childBlocks to the recursion to continue to the more depth
        puts("");
        puts("");
        Blocks.clear();
        print(newBlocks);
    }
}

int main(){
    int num[50];// = {1,4,7,10,17,21,31,25,19,20,28,42};
    // int num[] = {5,10,15,20,25,28,30,50,55,60,65,70,75,80,85,90,95};


    printf("Pointers: ");
    scanf("%d", &numberOfPointers);

    FILE *p;
    p = fopen("input.txt", "r");
    freopen("output.txt", "w", stdout);


    vector < Block* > Blocks;


    int ch;
    int i = 0;
    int totalValues = 0;
    //input from file
    printf("1.Insert a value\n2.Print the tree\n3.Delete a value\n");
    while(fscanf(p, "%d", &ch)!=EOF){

        if(ch==1){
            fscanf(p, "%d", &num[i]);
            printf("**** Insert %d ****\n\n", num[i]);
            insertNode(rootBlock, num[i]);
            i++;
            totalValues++;

        }else if(ch==2){
            Blocks.clear();
            Blocks.push_back(rootBlock);
            print(Blocks);
            puts("");

        }else if(ch==3) {
            int val;
            fscanf(p, "%d", &val);
            if(totalValues==0) {
                printf("Sorry! There is no more data to be deleted!!");
                continue;

            }
            printf("---- Delete %d ----\n\n", val);
            dataFound = false;
            deleteNode(rootBlock, val, 0);
            totalValues--;
        }
    }
    //input manually
     while(scanf("%d", &ch)!=EOF){


        if(ch==1){
            scanf("%d", &num[i]);
            printf("**** Insert %d ****\n\n", num[i]);
            insertNode(rootBlock, num[i]);
            i++;
            totalValues++;

        }else if(ch==2){
            Blocks.clear();
            Blocks.push_back(rootBlock);
            print(Blocks);
            puts("");

        }else if(ch==3) {
            int val;
            scanf("%d", &val);
            if(totalValues==0) {
                printf("Sorry! There is no more data to be deleted!!");
                continue;

            }
            printf("---- Delete %d ----\n\n", val);
            dataFound = false;
            deleteNode(rootBlock, val, 0);
            totalValues--;
        }
    }


    return 0;
}
