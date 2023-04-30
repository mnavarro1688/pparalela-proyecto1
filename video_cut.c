#include <iostream>
#include <opencv2/opencv.hpp>

struct frames {
    int index;
    char name[15];
    struct frames* next_frame;
};

struct clips {
    int index;
    char name[15];
    struct clips* next_clip;
    struct frame* first_frame; 
};

struct cuts {
    int cut_frame;
    struct cuts* next_cut;
};

typedef struct frames* Frame_List;
typedef struct clips* Clip_List;
typedef struct cuts* Cut_List;

void add_frame(Frame_List* list, frames* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct frames* last_frame = *list;
        while (last_frame->next_frame != NULL) {
            last_frame = last_frame->next_frame;
        }
        last_frame->next_frame = data;
    }
}

void add_cut(Cut_List* list, cuts* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct cuts* last_cut = *list;
        while (last_cut->next_cut != NULL) {
            last_cut = last_cut->next_cut;
        }
        last_cut->next_cut = data;
    }
}

void add_clip(Clip_List* list, clips* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct clips* last_clip = *list;
        while (last_clip->next_clip != NULL) {
            last_clip = last_clip->next_clip;
        }
        last_clip->next_clip = data;
    }
}

void sort_cuts(Cut_List* list) {
    //ordenar la lista de menor a mayor con respecto al cut_frame
}

float battachary(float v1, float v2) {
    //return op batacharrya
}

int main() //fatla agregar los args que recibe el programa archivo de video entrada, archivo de salida txt
{
    Frame_List all_frames = NULL;
    Clip_List all_clips = NULL;
    Cut_List aall_cuts = NULL;
    
    frames new_frame;
    clips new_clip;
    cuts new_cut;

    //Declarar aqui las variables restantes

    /*Aca agregamos el codigo para cargar el video y cortarlo en frames.
        tomar en cuenta que cada frame lo agregamos a la lista de frames all_frames
        donde:  index: sera el valor de la variable count en el ciclo de creacion de frames
                name: el nombre explicito que tiene el archivo 'ejemplo##.jpg'

                add_frame(&all_frames,new_frame);
    */

    /*En esta seccion leemos cada frame y lo convertimos a hsv para obtener los valores a utilizar para calcualar batthacharya
        recordemos que battacharya es la sumatoria de 2 variables, por lo que aca trabajamos con una posicion de la lista y la siguiente    
        Luego de calculada el bataccharya de 2 frames continuos tenemos que agregar el condicional para determinar si es un frame de corte
            de ser frame de corte hay que agregarlo a la lista all_cuts donde:
                cut_frame: es el index del frame actual
                next_cut: seria null

                for(int i = 0 , i < width, i++)
                    for(int j = 0, j < heigth, j++)
                        valor hsv para img1, img2

                        img1: frame actual 
                        img2: frame siguiente

                sum_batt=sum_batt + batacharrya (im1, im2)

        En la version paralelizada agregar codigo para que todos los procesos envien su lista de cuts a un proceso X
        este los una y los ordene de menor a mayor con respecto a la variable  cut_frame y lo envie de vuelta a todos los procs
    */

    /*Continuamos con la reconstruccion de los clips, ya con la lista all_cuts ordenada se procede a la regeneracion de los clips
        mientras se genera el clip ir aregandolos a la lista all_clips
        (aca debemos idear un algoritmo para poder implementar paralelizacion en la regeneracion de clips)
            incluir una varible o un metodo para poder dividir la cantidad de cuts entre los procesos y cada proc
            vaya generando los clips

            contador cuts = 0
            Cut_list  total_cuts = all_cuts
            while(total_cuts != NULL){
                cuts++
                total_cuts = total_cuts -> next_cut
            }

            sort_clips(&all_clips) enviarlo a los demas procesos 

            part_size = cuts / size
            my_first = rank * part_size
            my_last = (rank + 1) * part_size

            if(rank = size-1)
                my_last = cuts


    */

    /*dar formato al archivo de salida imprimiendo lo que pide el proyecto
    * cantidad de clips y los frames que abarca....
    * El video tiene un total de ### clips divididos de la siguiente forma
    * 
    *   Clip #:
    *       
    */


}