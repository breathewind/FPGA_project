/*************************************************************
 *           Author: Wenlong Wang, Xin Guan, Xiaoying Chen
 *      Create date: 07/04/2017
 * Modifiacion date: 25/04/2017
 *      Description: Translate script to program structure
 *************************************************************/
//#define debug

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define COMP_NAME_SIZE 40

#define PORT_NAME_SIZE 40
#define PORT_TYPE_SIZE 40
#define PORT_DIRECTION_SIZE 10
#define BUFFER_SIZE 40

struct port_info{
    char name[PORT_NAME_SIZE];
    char type[PORT_TYPE_SIZE];
    int id;
    char direction[PORT_DIRECTION_SIZE];
};

struct component_info{ // Data structure storing component inforamtion
    int comp_id; // Component ID
    int comp_ip; // IP ID corresponding to component
    char name[COMP_NAME_SIZE];
    int input_port_num;
    int output_port_num;
    struct port_info *port_list;
} *comp_list, top_entity;

struct signal_info{ // Data structure storing signal inforamtion
    int signal_id; // Signal ID
    int source_id; // Source component ID
    int source_port_id; // Source port ID
    int dest_id; // Destination component ID
    int dest_port_id; // Destination port ID
    int redundant;
} *signal_list;

int component_count; // Component counter
int signal_count; // Signal counter
int ip_count;
int* instantance_count;

//Function 01: Manipulate processing scripts to data structures
int script_translator(char* component_filepath, char* signal_filepath);
//Function 02: Print data stored inside data structures
void print_info();
//Function 03: Generate vhdl code
int vhdlmerge();

int main(int argc, char* argv[]){
    
    if (argc < 4) {
        printf("Error_1: few input arguments.\n");
        printf("Usage: ./run.x component_list signal_list\n");
        return 1;
    }
    
    if(script_translator(argv[1], argv[2])){
        return 1;
    }
    
    print_info();
    if(vhdlmerge(argv[3]))
    {
        return 1;
    }
    
    return 0;
}

/***********************************************************************
 *             Name: script_translator
 *      Function ID: 01
 *      Create date: 07/04/2017
 * Modifiacion date: 25/04/2017
 *      Description: Manipulate processing scripts to data structure
 ***********************************************************************/
int script_translator(char* component_filepath, char* signal_filepath){
    FILE *file; // File pointer
    //    char command_buffer[20]; // Buffer to store command read from script
    int component_index; // Index of component (used when capture component information from script)
    int signal_index; // Index of signal (used when capture signal information from script)
    
    int temp_comp_id;
    char temp_port_type[PORT_TYPE_SIZE];
    char temp_port_type_part2[PORT_TYPE_SIZE];
    char temp_port_type_part3[PORT_TYPE_SIZE];
    char buffer[BUFFER_SIZE];
    
    int i, j;
    int temp_int1, temp_int2;
    int temp_comp_count;
    int pointer_pos, last_pointer_pos;
    
    int temp_1, temp_2, temp_3, temp_4, temp_5; // Parameters for verifying script format
    //    char temp_6[10], temp_7[10]; // Parameters for verifying script format
    
    component_count = 0;
    component_index = 0;
    temp_comp_count = 0;
    signal_count = 0;
    signal_index = 0;
    ip_count = 0;
    
    file = fopen(component_filepath, "r"); // Open file for reading
    
    if (!file) {
        printf("Error_2: Fail to open file %s.\n", component_filepath);
        return 2;
    }
    
    printf("Info: %s syntax check starts... \n", component_filepath);
    while(fscanf(file, "%d", &temp_comp_id)!=EOF){
        int temp_sum;
        ip_count++;
        fscanf(file, "%s %d %d", buffer, &temp_int1, &temp_int2);
        temp_sum = temp_int1+temp_int2;
        for (i=0; i<temp_sum; i++) {
            fscanf(file, "%s", buffer); // port name
            fscanf(file, "%s", buffer); // port type
            strncpy(temp_port_type, buffer, 16);
            temp_port_type[16]='\0';
            if(!strcmp(temp_port_type,"std_logic_vector")){ // if port type is std_logic_vector
                fscanf(file, "%s", temp_port_type_part2);
                fscanf(file, "%s", temp_port_type_part3);
            }
            fscanf(file, "%d %s", &temp_int1, buffer);
        }
        
        pointer_pos = 0;
        fscanf(file, "%s", buffer);
        while (buffer[0] >47 && buffer[0] < 58) {
            component_count ++;
            last_pointer_pos = pointer_pos;
            pointer_pos = ftell(file);
            if (fscanf(file, "%s", buffer)==EOF) { //If there's no more data
                last_pointer_pos = ftell(file);
                component_count++;
                break;
            }
        }
        component_count--;
        fseek(file, last_pointer_pos, SEEK_SET);
    }
    component_count--;
    printf("Info: Syntax check finished. #IP = %d #Component = %d\n", ip_count-1, component_count);
    fseek(file, 0, SEEK_SET);
    
    comp_list = (struct component_info*)malloc(sizeof(struct component_info)*component_count);
    instantance_count = (int *)malloc(sizeof(int)*ip_count);
    
    fscanf(file, "%d %s %d %d", &top_entity.comp_ip, top_entity.name, &top_entity.input_port_num, &top_entity.output_port_num);
    top_entity.port_list = (struct port_info*)malloc(sizeof(struct port_info)*(top_entity.input_port_num+top_entity.output_port_num));
    for(j=0; j<top_entity.input_port_num+top_entity.output_port_num; j++){
        fscanf(file, "%s", top_entity.port_list[j].name);
        fscanf(file, "%s", top_entity.port_list[j].type);
        strncpy(temp_port_type, top_entity.port_list[j].type, 16);
        temp_port_type[16]='\0';
        if(!strcmp(temp_port_type,"std_logic_vector")){
            fscanf(file, "%s", temp_port_type_part2);
            fscanf(file, "%s", temp_port_type_part3);
            sprintf(top_entity.port_list[j].type, "%s %s %s", top_entity.port_list[j].type, temp_port_type_part2, temp_port_type_part3);
        }
        fscanf(file, "%d %s", &top_entity.port_list[j].id, top_entity.port_list[j].direction);
    }
    fscanf(file, "%d", &top_entity.comp_id);
    
    
    for (i=1; i<ip_count; i++) {
        struct component_info component;
        fscanf(file, "%d %s %d %d", &component.comp_ip, component.name, &component.input_port_num, &component.output_port_num);
        component.port_list = (struct port_info*)malloc(sizeof(struct port_info)*(component.input_port_num+component.output_port_num));
        for(j=0; j<component.input_port_num+component.output_port_num; j++){
            fscanf(file, "%s", component.port_list[j].name);
            fscanf(file, "%s", component.port_list[j].type);
            strncpy(temp_port_type, component.port_list[j].type, 16);
            temp_port_type[16]='\0';
            if(!strcmp(temp_port_type,"std_logic_vector")){
                fscanf(file, "%s", temp_port_type_part2);
                fscanf(file, "%s", temp_port_type_part3);
                sprintf(component.port_list[j].type, "%s %s %s", component.port_list[j].type, temp_port_type_part2, temp_port_type_part3);
            }
            fscanf(file, "%d %s", &component.port_list[j].id, component.port_list[j].direction);
        }
        pointer_pos = ftell(file);
        instantance_count[i] = 0;
        fscanf(file, "%s", buffer);
        while (buffer[0] >47 && buffer[0] < 58) {
            instantance_count[i]++;
            if (fscanf(file, "%s", buffer)==EOF) { //If there's no more data
                instantance_count[i]++;
                break;
            }
        }
        instantance_count[i]--;
        fseek(file, pointer_pos, SEEK_SET);
        
        //        printf("instantance_count: %d\n",instantance_count[i]);
        for(j=0;j<instantance_count[i];j++){
            fscanf(file, "%d", &comp_list[component_index].comp_id);
            strcpy(comp_list[component_index].name ,component.name);
            comp_list[component_index].comp_ip = component.comp_ip;
            comp_list[component_index].input_port_num = component.input_port_num;
            comp_list[component_index].output_port_num = component.output_port_num;
            comp_list[component_index].port_list = component.port_list;
            component_index++;
        }
        
    }
    fclose(file); // Close file pointer
    
    
    file = fopen(signal_filepath, "r"); // Open file for reading
    
    printf("Info: %s syntax check starts... \n", signal_filepath);
    while (fscanf(file, "%d %d %d %d %d", &temp_1, &temp_2, &temp_3, &temp_4, &temp_5)!=EOF) {
        signal_count++;
    }
    printf("Info: Syntax check finished. #Signal = %d\n", signal_count);
    
    signal_list = (struct signal_info*)malloc(sizeof(struct signal_info)*signal_count);
    fseek(file, 0, SEEK_SET);
    
    for (i=0; i<signal_count; i++) {
        fscanf(file, "%d %d %d %d %d", &signal_list[i].signal_id,
               &signal_list[i].source_id,
               &signal_list[i].source_port_id,
               &signal_list[i].dest_id,
               &signal_list[i].dest_port_id);
        signal_list[i].redundant = 0;
    }
    
    fclose(file); // Close file pointer
    
    return 0;
}

/***********************************************************************
 *             Name: print_info
 *      Function ID: 02
 *      Create date: 09/04/2017
 * Modifiacion date: 25/04/2017
 *      Description: Print data stored inside data structures
 ***********************************************************************/
void print_info(){
    int i,j;
    printf("====================================================\n");
    printf("-- Print all data in top entity:\n");
    printf("%s_%d\tip:%d\n", top_entity.name, top_entity.comp_id, top_entity.comp_ip);
    printf("Input port: %d\n", top_entity.input_port_num);
    printf("Output port: %d\n", top_entity.output_port_num);
    printf("Port information:\n");
    for (j=0; j<top_entity.input_port_num+top_entity.output_port_num; j++) {
        printf("%s\t%s\t%d\t%s\n", top_entity.port_list[j].name,
               top_entity.port_list[j].type,
               top_entity.port_list[j].id,
               top_entity.port_list[j].direction);
    }
    printf("====================================================\n");
    printf("-- Print all data in data structure:\n");
    for (i=0; i<component_count; i++) {
        printf("%s_%d\tip:%d\n", comp_list[i].name ,comp_list[i].comp_id, comp_list[i].comp_ip);
        printf("Input port: %d\n", comp_list[i].input_port_num);
        printf("Output port: %d\n", comp_list[i].output_port_num);
        printf("Port information:\n");
        for (j=0; j<comp_list[i].input_port_num+comp_list[i].output_port_num; j++) {
            printf("%s\t%s\t%d\t%s\n", comp_list[i].port_list[j].name,
                   comp_list[i].port_list[j].type,
                   comp_list[i].port_list[j].id,
                   comp_list[i].port_list[j].direction);
        }
        printf("----------------------------------------------------\n");
    }
    
    for (i=0; i<signal_count; i++) {
        printf("Info: Signal_%d\tsource id:      %d\t|port id: %d\n",
               signal_list[i].signal_id,
               signal_list[i].source_id,
               signal_list[i].source_port_id);
        printf("               \tdestination id: %d\t|port id: %d\n",
               signal_list[i].dest_id,
               signal_list[i].dest_port_id);
    }
}

/***********************************************************************
 *             Name: vhdlmerge
 *      Function ID: 03
 *      Create date: 25/04/2017
 * Modifiacion date: 25/04/2017
 *      Description: Generate vhdl code
 ***********************************************************************/
int vhdlmerge(char *filename)
{
    char port_name[PORT_NAME_SIZE];
    char port_type[PORT_TYPE_SIZE];
    int in_port_cnt;
    int external_port_cnt = top_entity.input_port_num + top_entity.output_port_num;
    int i,j,k,m,n;
    int position;
    
    FILE* pFILE;
    pFILE= fopen(filename,"w+");
    if(pFILE==NULL)
    {
        printf("open file mergedVHDL.txt failed!\n");
        exit(1);
    }
    fprintf(pFILE,"library IEEE;\n");
    fprintf(pFILE,"use IEEE.STD_LOGIC.ALL;\n\n");
    fprintf(pFILE,"ENTITY %s IS\n",top_entity.name);
    fprintf(pFILE,"    PORT (\n");
    
    m = top_entity.input_port_num + top_entity.output_port_num -1;

    for (i=0;i<m;i++)
    {
        fprintf(pFILE,"          %s : %s %s;\n",top_entity.port_list[i].name,top_entity.port_list[i].direction,top_entity.port_list[i].type);
    }
    i = (top_entity.input_port_num + top_entity.output_port_num - 1);
    fprintf(pFILE,"          %s : %s %s\n",top_entity.port_list[i].name,top_entity.port_list[i].direction,top_entity.port_list[i].type);
    fprintf(pFILE,"          );\n");
    fprintf(pFILE,"END %s;\n\n",top_entity.name);
     // entity end
    
    fprintf(pFILE,"ARCHITECTURE struct OF %s IS\n",top_entity.name);
    fprintf(pFILE,"    component %s\n",comp_list[0].name);
    fprintf(pFILE,"    PORT (\n");
    for (j=0;j<(comp_list[0].input_port_num + comp_list[0].output_port_num - 1);j++)
    {
        fprintf(pFILE,"          %s : %s %s;\n",comp_list[0].port_list[j].name,comp_list[0].port_list[j].direction,comp_list[0].port_list[j].type);
    }
    j = (comp_list[0].input_port_num + comp_list[i].output_port_num - 1);
    fprintf(pFILE,"          %s : %s %s\n",comp_list[0].port_list[j].name,comp_list[0].port_list[j].direction,comp_list[0].port_list[j].type);
    fprintf(pFILE,"         );\n");
    fprintf(pFILE,"    end component;\n\n");
    for (i = 1;i < component_count;i++)
    {
        for (m = 0;m < i;m++)
        {
            if (comp_list[i].comp_ip != comp_list[m].comp_ip);
            else break;
        }
        if (m == i)
        {
            fprintf(pFILE,"    component %s\n",comp_list[i].name);
            fprintf(pFILE,"    PORT (\n");
            for (j=0;j<(comp_list[i].input_port_num + comp_list[i].output_port_num - 1);j++)
            {
                fprintf(pFILE,"          %s : %s %s;\n",comp_list[i].port_list[j].name,comp_list[i].port_list[j].direction,comp_list[i].port_list[j].type);
            }
            j = (comp_list[i].input_port_num + comp_list[i].output_port_num - 1);
            fprintf(pFILE,"          %s : %s %s\n",comp_list[i].port_list[j].name,comp_list[i].port_list[j].direction,comp_list[i].port_list[j].type);
            fprintf(pFILE,"         );\n");
            fprintf(pFILE,"    end component;\n\n");
        }
    }
    
    /*******************************************************************************************/
    
    // mark the reduntdant signals (when 1 output signal connects to more than 1 port)
    for (i=0; i<signal_count; i++)
    {
        for(j=0;j<i;j++)
        {
           if(signal_list[j].source_id == signal_list[i].source_id && signal_list[j].source_port_id == signal_list[i].source_port_id)
           {
               signal_list[i].signal_id == signal_list[j].signal_id;
               signal_list[i].redundant = 1;
           }
        }
    }
        
    // internal signal list
    for (i=0; i<signal_count; i++)
    {
        if(signal_list[i].redundant == 1)
            continue;   // do not print the redundant signal defination
        
        strcpy(port_type,"\0");
        
        // find the port type
        if(signal_list[i].dest_id == 0) // find the signal type through top level entity declarition
        {
            for(j=0;j<external_port_cnt;j++)
            {
                if(top_entity.port_list[j].id == signal_list[i].dest_port_id)
                {
                    strcpy(port_type,top_entity.port_list[j].type);
                    break;
                }
            }
        }
        else
        {
            for(j=0;j<component_count;j++)
            {
                m = 0;
                if(comp_list[j].comp_id == signal_list[i].dest_id)
                {
                    in_port_cnt = comp_list[j].input_port_num;
                    for(k=0;k<in_port_cnt;k++)
                    {
                        if(comp_list[j].port_list[k].id == signal_list[i].dest_port_id)
                        {
                            strcpy(port_type,comp_list[j].port_list[k].type);
                            m = 1;  // flag to begin processing the next signal
                            break;
                        }
                    }
                }
                if(m==1)
                    break;
            }
        }
        fprintf(pFILE,"signal s_%d : %s;\n",signal_list[i].signal_id,port_type);
    }
    
    fprintf(pFILE,"\n begin\n\n");
    
    // external inputs/outputs to internal signal assignments
    for(k=0;k<signal_count;k++)
    {
        if(signal_list[k].source_id == 0) // external input
        {
            for(n=0;n<external_port_cnt;n++) // find the port name of the top entity associated with the signal
            {
                if(signal_list[k].source_port_id == top_entity.port_list[n].id)
                    strcpy(port_name,top_entity.port_list[n].name);
            }
            // signal assignment
            fprintf(pFILE,"s_%d <= %s;\n",signal_list[k].signal_id,port_name);
        }
        else if(signal_list[k].dest_id == 0) // external output
        {
            for(n=0;n<external_port_cnt;n++)
            {
                if(signal_list[k].dest_port_id == top_entity.port_list[n].id)
                    strcpy(port_name,top_entity.port_list[n].name);
            }
            fprintf(pFILE,"s_%d <= %s;\n",signal_list[k].signal_id,port_name);
        }
    }
    fprintf(pFILE,"\n");
    
    // port map
    for (i=0; i<component_count; i++)
    {
        int cnt = comp_list[i].input_port_num + comp_list[i].output_port_num; //number of ports of the component
        fprintf(pFILE,"%s_%d : %s port map(",comp_list[i].name,comp_list[i].comp_id,comp_list[i].name);
        for(k=0;k<signal_count;k++)
        {
            strcpy(port_name,"\0");
            if(comp_list[i].comp_id == signal_list[k].source_id)
            {    // find the corresponding port name
                strcpy(port_name,"No name found");
                for(m=0;m<cnt;m++)
                {
                    if(comp_list[i].port_list[m].id == signal_list[k].source_port_id)
                    {
                        strcpy(port_name,comp_list[i].port_list[m].name);
                        break;
                    }
                    if(strlen(port_name) == 0)
                    {
                        printf("Error4:unable to find port name!\n");
                        return 4;
                    }
                }
                fprintf(pFILE,"%s => s_%d, ",port_name,signal_list[k].signal_id);
            }
            else if(comp_list[i].comp_id == signal_list[k].dest_id)
            {
                strcpy(port_name,"No name found");
                
                for(m=0;m<cnt;m++)
                {
                    if(comp_list[i].port_list[m].id == signal_list[k].dest_port_id)
                    {
                        strcpy(port_name,comp_list[i].port_list[m].name);
                        break;
                    }
                    if(strlen(port_name) == 0)
                    {
                        fprintf(pFILE,"Error4:unable to find port name!\n");
                        return 4;
                    }
                }
                fprintf(pFILE,"%s => s_%d, ",port_name,signal_list[k].signal_id);
            }
        }
        position = ftell(pFILE)-2;
        fseek(pFILE, position, SEEK_SET);
        fprintf(pFILE,");\n");
    }
    /*************************************************************************************/
    
    fprintf(pFILE,"\nEND struct;\n");
    fclose(pFILE);
    return 0;
}
