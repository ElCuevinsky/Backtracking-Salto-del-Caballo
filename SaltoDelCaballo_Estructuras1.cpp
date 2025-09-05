#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>  //para manejo de archivos
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

using namespace std;

void hacerTablero(int n, int paso_actual);
bool solve(int x, int y, int k, int n);
bool allegrao(int n);
void despejarGrafico();
bool archivo(int n);  //función para manejar archivos

//se configuran las variables a usar para el tablero y los movimientos del caballo
const int TOPE = 100;   //el tope puesto es 100, pero si se pone ese numero tardará mucho en encontrar el recorrido, por lo que se recomienda usar un número menor           
int tablero[TOPE][TOPE];              
int dx[8]={2,1,-1,-2,-2,-1,1,2};//iteraciones de los movimientos del caballo en x
int dy[8]={1,2,2,1,-1,-2,-2,-1};//iteraciones de los movimientos del caballo en y

// Variables para la interfaz gráfica
const int CELL_SIZE=60;
const int MARGIN=50;
int ANIMATION_DELAY=500; // ms entre movimientos
ALLEGRO_DISPLAY* display=nullptr;   // ventana de visualización
ALLEGRO_FONT* font=nullptr;     // fuente para el texto
ALLEGRO_TIMER* timer=nullptr;       // temporizador para animaciones
ALLEGRO_EVENT_QUEUE* event_queue=nullptr;       // cola de eventos para manejar entradas y temporizadores

void hacerTablero(int n,int paso_actual){
    al_clear_to_color(al_map_rgb(255,255,255));     // limpiar pantalla con color blanco
    
    int board_size=n*CELL_SIZE;
    int start_x=(al_get_display_width(display)-board_size)/2;     // calcular posición inicial de x para centrar el tablero
    int start_y=(al_get_display_height(display)-board_size)/2;    // calcular posición inicial de y para centrar el tablero

    // Dibujar tablero
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            ALLEGRO_COLOR cell_color;       
            // Determinar el color base de la celda
            if(tablero[i][j]!=-1){
                // Celda visitada - verde oscuro/claro según el color original
                // Usar dos tonos de verde para las celdas visitadas, simulando el patrón de ajedrez
                if((i+j)%2==0){
                    cell_color=al_map_rgb(144, 238, 144); // verde claro
                } else {
                    cell_color=al_map_rgb(34, 139, 34);   // verde oscuro
                }
            } 
            else{
                // Celda no visitada - colores originales
                // Colores de tablero de ajedrez: blanco y marrón
                if((i+j)%2==0){
                    cell_color=al_map_rgb(240, 217, 181); // celda clara
                } else {
                    cell_color=al_map_rgb(181, 136, 99);  // celda oscura
                }
            }
            
            // Resaltar la posición actual del caballo en amarillo
            if(tablero[i][j]==paso_actual-1){
                cell_color=al_map_rgb(255,255,0); // amarillo
            }

            al_draw_filled_rectangle(
                start_x+j*CELL_SIZE,        // posición x de la celda
                start_y+i*CELL_SIZE,        // posición y de la celda
                start_x+(j+1)*CELL_SIZE,        // posición x final de la celda
                start_y+(i+1)*CELL_SIZE,        // posición y final de la celda
                cell_color      
            );
            
            al_draw_rectangle(
                start_x+j*CELL_SIZE,        // posición x de la celda
                start_y+i*CELL_SIZE,        // posición y de la celda
                start_x+(j+1)*CELL_SIZE,        // posición x final de la celda
                start_y+(i+1)*CELL_SIZE,        // posición y final de la celda
                al_map_rgb(0,0,0),1
            );
            
            // Dibujar número si la casilla ha sido visitada
            if(tablero[i][j]!=-1){
                ALLEGRO_COLOR text_color=al_map_rgb(0,0,0); // texto siempre negro
                
                char str[10];
                sprintf(str,"%d",tablero[i][j]);
                
                al_draw_text(
                    font,
                    text_color,
                    start_x+j*CELL_SIZE+CELL_SIZE/2,        // posición x del texto
                    start_y+i*CELL_SIZE+CELL_SIZE/2-10,     // posición y del texto
                    ALLEGRO_ALIGN_CENTER,
                    str
                );
                
                // Dibujar caballo en la posición actual
                if(tablero[i][j]==paso_actual-1){
                    al_draw_text(
                        font,
                        al_map_rgb(0,0,0), // caballo negro para mejor contraste
                        start_x+j*CELL_SIZE+CELL_SIZE/2,        // posición x del texto
                        start_y+i*CELL_SIZE+CELL_SIZE/2+15,     // posición y del texto
                        ALLEGRO_ALIGN_CENTER,
                        "♞"        //emoji del caballo (no jala)
                    );
                }
            }
        }
    }

    // Líneas entre saltos 
    if(paso_actual>1){
        int prev_x=-1,prev_y=-1,curr_x=-1,curr_y=-1;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(tablero[i][j]==paso_actual-2){prev_x=j;prev_y=i;}
                if(tablero[i][j]==paso_actual-1){curr_x=j;curr_y=i;}
            }
        }
        if(prev_x!=-1&&prev_y!=-1&&curr_x!=-1&&curr_y!=-1){
            int x1=start_x+prev_x*CELL_SIZE+CELL_SIZE/2;
            int y1=start_y+prev_y*CELL_SIZE+CELL_SIZE/2;
            int x2=start_x+curr_x*CELL_SIZE+CELL_SIZE/2;
            int y2=start_y+curr_y*CELL_SIZE+CELL_SIZE/2;
            al_draw_line(x1,y1,x2,y2,al_map_rgb(0,0,255),3);
        }
    }
    
    // Dibujar información
    al_draw_textf(
        font,       
        al_map_rgb(0,0,0),  //define el color de la fuente en negro  
        al_get_display_width(display)/2,    //toma el centro del tablero
        20, 
        ALLEGRO_ALIGN_CENTER,               
        "Recorrido del Caballo - Paso: %d/%d",paso_actual,n*n  //escribe en el este texto
    );
    
    // Control de velocidad (nuevo - añadido sin modificar tus comentarios existentes)
    al_draw_textf(
        font,
        al_map_rgb(0,0,0),
        al_get_display_width(display)/2,
        al_get_display_height(display)-20,
        ALLEGRO_ALIGN_CENTER,
        "Velocidad: %dms (Flechas: ↑↓)",ANIMATION_DELAY
    );
    
    al_flip_display();                      //vuelve visible los cambios hechos en el display

    // Manejo de eventos durante espera (nuevo - manteniendo estructura original)
    double wait_until=al_get_time()+(ANIMATION_DELAY/1000.0);
    while(true){
        double remaining=wait_until-al_get_time();
        if(remaining<=0)break;
        ALLEGRO_EVENT event;
        bool got_event=al_wait_for_event_timed(event_queue,&event,remaining);
        if(got_event){
            if(event.type==ALLEGRO_EVENT_KEY_DOWN){
                if(event.keyboard.keycode==ALLEGRO_KEY_UP){
                    ANIMATION_DELAY=max(50,ANIMATION_DELAY-100);
                }else if(event.keyboard.keycode==ALLEGRO_KEY_DOWN){
                    ANIMATION_DELAY+=100;
                }
                wait_until=al_get_time()+(ANIMATION_DELAY/1000.0);
            }else if(event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
                despejarGrafico();
                exit(0);
            }
        }
    }
}

//resolver el algoritmo con backtrack
bool solve(int x,int y,int k,int n){
    if(k==n*n)return true;  //si el numero de movimientos es igual al tamaño del tablero, retorna true

    vector<pair<int,int>> movimientos;      //define la matriz donde se guardan los movimientos

    for(int i=0;i<8;i++){
        int nx=x+dx[i];     //le suma el valor de la coordenada x mas la iteración del caballo en x dentro un for de 8 teniendo las 8 posibles posiciones en cuenta 
        int ny=y+dy[i];     //le suma el valor de la coordenada y mas la iteración del caballo en y dentro un for de 8 teniendo las 8 posibles posiciones en cuenta
        if(nx>=0&&nx<n&&ny>=0&&ny<n&&tablero[nx][ny]==-1){      //valida que el caballo no haya entrado a una posición fuera del tablero 
            int contar=0;           //define un contador
            for(int j=0;j<8;j++){
                int nnx=nx+dx[j];       //suma el resultado de la suma de la coordenada de x mas la iteración del caballo en x dentro un for de 8 teniendo las 8 posibles posiciones en cuenta 
                int nny=ny+dy[j];   //suma el resultado de la suma de la coordenada de x mas la iteración del caballo en x dentro un for de 8 teniendo las 8 posibles posiciones en cuenta
                if(nnx>=0&&nnx<n&&nny>=0&&nny<n&&tablero[nnx][nny]==-1){ //valida que la siguiente posición no tenga quede fuera del tablero
                    contar++;           //aumenta el contador
                }
            }
            movimientos.emplace_back(contar, i);    //guarda en la cola movimientos el numero de movimiento y la iteración 
        }
    }

    sort(movimientos.begin(), movimientos.end());   //ordena la cola de movimientos

    for (auto& movimiento:movimientos) {
        int i=movimiento.second;        //define i y le da el valor del movimiento segun los segundos que han pasado en el reloj
        int nx=x+dx[i];                 //define nx con el valor de x + el valor de la iteración de dx
        int ny=y+dy[i];                 //define ny con el valor de y + el valor de la iteración de dy
        tablero[nx][ny]=k;              //le da a las casillas del tablero el numero de movimiento es.
        
        // Dibujar el estado actual
        hacerTablero(n,k+1);

        if(solve(nx,ny,k+1,n)){        //mientras  que sea verdadero, se llamara a la funcion solve mandandole los nuevos valores de x, y, la cantidad de movimientos+1 y el tamaño del tabloer
            return true;
        }
        tablero[nx][ny]=-1;     //define la posición de la ultima x y la ultima y como -1

        // Dibujar el retroceso
        hacerTablero(n,k);
    }
    return false;
}

//valida si alegro esta bien inicializado
bool allegrao(int n){
    if(!al_init()){                                 //valida si no se puede acceder a alegro
        cerr<<"Error al inicializar Allegro"<<endl; //imprime un mensaje de error
        return false;                               
    }

    if(!al_install_keyboard()){                    // Inicializar teclado 
        cerr<<"Error al inicializar teclado"<<endl;
        return false;
    }

    if(!al_init_primitives_addon()){                    //valida si no se añadieron los addons necesarios para el funcionamiento 
        cerr<<"Error al inicializar primitivas"<<endl;  //imprime un mensaje de error
        return false;                                   
    }

    if(!al_init_font_addon()){                                      //valida si no estan añadidas las fuentes 
        cerr<<"Error al inicializar addon de fuentes"<<endl;        //imprime un mensaje de error
        return false;                                               
    }

    if(!al_init_ttf_addon()){                               //valida si no estan añadidas los addons de fuentes     
        cerr<<"Error al inicializar addon de TTF"<<endl;    //imprime un mensaje de error
        return false;                                       
    }

    int board_size=n * CELL_SIZE;               //define el tamano del tablero como la cantidad de casillas multiplicado por el tamaño de cada celda
    int window_width=board_size+2 * MARGIN;     //define margenes
    int window_height=board_size+2 * MARGIN+40; // Espacio extra para texto

    display=al_create_display(window_width, window_height); //genera el dysplay
    if(!display){                                           //valida que no se haya generado el display
        cerr<<"Error al crear display"<<endl;               //imprime mensaje de error
        return false;                                   
    }
    
    font=al_load_ttf_font("arial.ttf",16,0);            //define la fuente
    if(!font){                                          //valida si no hay fuente
        cerr<<"Error al cargar fuente"<<endl;           //imprime mensaje de error 
        return false;                                   
    }

    timer=al_create_timer(1.0 / 60);            //define un reloj
    if(!timer){                                 //valida si no hay reloj
        cerr<<"Error al crear timer"<<endl;     //imprime mensaje de error 
        return false;
    }

    event_queue=al_create_event_queue();                //define la cola de eventos
    if(!event_queue){                                   //valida si no hay cola
        cerr<<"Error al crear cola de eventos"<<endl;   //imprime mensaje de error
        return false;
    }

    al_register_event_source(event_queue,al_get_display_event_source(display)); //registra en la cola de eventos los cambios en el display
    al_register_event_source(event_queue,al_get_timer_event_source(timer)); //registra en la cola de eventos los cambios del reloj
    al_register_event_source(event_queue,al_get_keyboard_event_source()); //registrar eventos de teclado
    
    return true;
}
//cierre del apartado grafico
void despejarGrafico(){
    if (font)al_destroy_font(font);                         //cierra la fuente
    if (timer)al_destroy_timer(timer);                      //cierra el reloj
    if (event_queue)al_destroy_event_queue(event_queue);    //cierra la cola de eventos
    if (display)al_destroy_display(display);                //cierra el display
}

//función para manejar archivos
bool archivo(int n){
    ifstream entrada("caballo.txt");    //intenta abrir el archivo para lectura
    if(entrada.good()){     
                   //verifica si el archivo existe
        cout<<"Se encontró un archivo con solución. ¿Desea simular esa solución? (s/n): ";
        char opcion;
        cin>>opcion;
        if(opcion=='s'||opcion=='S'){
            int n_archivo;
            entrada>>n_archivo;
            if(n_archivo!=n){ // Verificar si el tamaño del tablero en el archivo coincide con el solicitado
                cout<<"El tamaño del tablero en el archivo no coincide. Se recalculará."<<endl;
                return false;
            }
            
            //se lee la matriz desde el archivo
            for(int i=0;i<n;i++)
                for(int j=0;j<n;j++)
                    entrada>>tablero[i][j];
            entrada.close();
            return true;
        }
        entrada.close();
    }
    return false;
}

int main() {
    int n;
    cout<<"Introduce tamaño n del tablero (<= "<<TOPE<<"): ";   //define el tamaño del tablero
    cin>>n;
    if(n<1||n>TOPE){                                     //valida que el tamaño del tablero sea menor a 0 o mayor a TPOE (100)
        cout<<"Dimensión inválida.\n";                  //muestra mensaje de error
        return 1;
    }

    float delay_seconds;                                
    cout<<"Introduce el tiempo de espera entre movimientos (en segundos): ";       //define el delay que habrá en la presentación de los movimientos
    cin>>delay_seconds;
    ANIMATION_DELAY=static_cast<int>(delay_seconds * 1000); // Convertir a milisegundos

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            tablero[i][j]=-1;   //inicializa el tablero con -1 y 0

    tablero[0][0]=0;

    //verifica si existe solución en archivo
    bool cargado=archivo(n);
    
    if(!allegrao(n)){           //valida si existe algun problema con lo grafico
        despejarGrafico();      //destruye todo lo grafico
        return 1;
    }
    
    if(!cargado){
        hacerTablero(n,1); // Mostrar estado inicial solo si no se cargó del archivo
        
        if (solve(0,0,1,n)){
            cout<<"Recorrido encontrado:\n";
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++)
                    cout<<tablero[i][j]<<"\t";         //muestra el resultado final de los movimientos del caballo
                cout<<"\n";
            }
            
            //guarda solución en archivo
            ofstream salida("caballo.txt");
            if(salida){
                salida<<n<<endl;   //guardar tamaño del tablero
                for(int i=0;i<n;i++){
                    for(int j=0;j<n;j++){
                        salida<<tablero[i][j]<<" ";
                    }
                    salida<<endl;
                }
                salida.close();
            }
            
            // Mostrar resultado final por 5 segundos
            hacerTablero(n,n*n+1);
            al_rest(5.0);
        } 
        else{
            cout<<"No hay solución para n = "<<n<<".\n";
            al_draw_text(
                font,
                al_map_rgb(255, 0, 0),                          //define la letra en color rojo
                al_get_display_width(display)/2,                //define la posición horizontal del mensaje
                al_get_display_height(display)/2,               //define la posición vertical del mensaje
                ALLEGRO_ALIGN_CENTER,
                "No hay solución para este tamaño de tablero"   //escribe un mensaje centrado en color rojo
            );
            al_flip_display();              //hace visibles los cambios visibles 
            al_rest(5.0);                   //hace una pausa de 5 segundos
        }
    }
    else{
        // Simular solución desde archivo
        for(int paso=1;paso<=n*n;paso++){
            hacerTablero(n,paso);
        }
        hacerTablero(n,n*n+1);
        al_rest(5.0);
    }
    
    //se ha completado el recorrido o no se encontró solución
    despejarGrafico();  
    return 0;
}