#include <allegro.h>
#include <winalleg.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>


volatile int close_button_pressed = FALSE;
void close_button_handler(void){
    close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

void configuracionValidacion(BITMAP *buffer,BITMAP *fondo,BITMAP *patron,BITMAP *cursor,FONT *MiFuente,HANDLE *hComm , BOOL *Status,DWORD *dwEventMask,DWORD *NoBytesRead,DCB *dcbSerialParams, COMMTIMEOUTS *timeouts){
    char ComPortName[] = "";
    char com[20] = "\\\\.\\COM";
    char comAux[20] = "\\\\.\\COM";
    int longitud = 0;

    do{
        strcpy(com,comAux);
        while(!close_button_pressed && !key[KEY_ENTER]){
            //Mostrar base
            //printf("X: %d Y: %d\n",mouse_x,mouse_y);//Posiciones del MOUSE

            blit(fondo,buffer,0,0,0,0,900,600);
            rectfill(buffer,40,40,190,190,makecol(231,238,237));//Color del sensor
            rectfill(buffer,230,40,380,190,makecol(231,238,237));//Color seleccionado
            blit(patron,buffer,0,0,425,20,900,600);//Tabla colores

            /*Cuadros de colores RGB Sensor y RGB Color*/

            rectfill(buffer,425,250,493,318,makecol(231,238,237));
            rectfill(buffer,503,250,571,318,makecol(231,238,237));
            rectfill(buffer,581,250,649,318,makecol(231,238,237));

            rectfill(buffer,659,250,727,318,makecol(231,238,237));
            rectfill(buffer,737,250,805,318,makecol(231,238,237));
            rectfill(buffer,815,250,883,318,makecol(231,238,237));

            longitud = strlen(com);
            //printf("%d\n",longitud);

            if(longitud < 12){
                if(keypressed()){
                    if(key[KEY_0])
                    {
                        strcat(com,"0");
                    }
                    if(key[KEY_1])
                    {
                        strcat(com,"1");
                    }
                    if(key[KEY_2])
                    {
                        strcat(com,"2");
                    }
                    if(key[KEY_3])
                    {
                        strcat(com,"3");
                    }
                    if(key[KEY_4])
                    {
                        strcat(com,"4");
                    }
                    if(key[KEY_5])
                    {
                        strcat(com,"5");
                    }
                    if(key[KEY_6])
                    {
                        strcat(com,"6");
                    }
                    if(key[KEY_7])
                    {
                        strcat(com,"7");
                    }
                    if(key[KEY_8])
                    {
                        strcat(com,"8");
                    }
                    if(key[KEY_9])
                    {
                        strcat(com,"9");
                    }
                    if(key[KEY_BACKSPACE])
                    {
                        strcpy(com,comAux);
                    }
                    clear_keybuf();

                }
            }
            else{
                    if(keypressed()){
                        if(key[KEY_BACKSPACE])
                        {
                            strcpy(com,comAux);
                        }
                    clear_keybuf();
                    }
            }


            textout_ex(buffer, MiFuente,com,80,10,makecol(0,0,0),-1);
            textout_ex(buffer, MiFuente, "COM:",10,10,makecol(0,0,0),-1);
            masked_blit(cursor,buffer,0,0,mouse_x,mouse_y,13,22);
            blit(buffer,screen,0,0,0,0,900,600);
        }


        strcpy(ComPortName,com);
        (*hComm) = CreateFile(ComPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);    // opening the serial port
        (*dcbSerialParams).DCBlength = sizeof((*dcbSerialParams));    // DCB = Data Control Block

        (*Status) = GetCommState((*hComm), &(*dcbSerialParams));    // retrieving the current settings

        //printf("%d\n",(*Status));
        //system("PAUSE()");

    }while(!(*Status) && !close_button_pressed);

        if(!close_button_pressed){
            (*dcbSerialParams).BaudRate = CBR_9600;
            (*dcbSerialParams).ByteSize = 8;
            (*dcbSerialParams).StopBits = ONESTOPBIT;
            (*dcbSerialParams).Parity = NOPARITY;

            (*Status) = SetCommState((*hComm), &(*dcbSerialParams));    // configuring the port according to dcb structures

            (*timeouts).ReadIntervalTimeout = 50;
            (*timeouts).ReadTotalTimeoutConstant = 50;
            (*timeouts).ReadTotalTimeoutMultiplier = 10;
            (*timeouts).WriteTotalTimeoutConstant = 50;
            (*timeouts).WriteTotalTimeoutMultiplier = 10;

            if (SetCommTimeouts((*hComm), &(*timeouts)) == FALSE){return;}

            (*Status) = SetCommMask((*hComm), EV_RXCHAR);    // configuring windows for reception
            (*Status) = WaitCommEvent((*hComm), &(*dwEventMask), NULL);    // waiting for the reception
            //Fin Configuración Serial Port
        }

}

int main ()
{
    /* *** Con estas lineas de codigo preparamos el entorno para graficos y sonidos *** */
    allegro_init();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED,900,600, 0, 0);
    install_mouse();
    LOCK_FUNCTION(close_button_handler);
    set_close_button_callback(close_button_handler);
    /* ******************************************************************************** */

    /*Inicializacion de variables*/
    BITMAP *buffer = create_bitmap(900,600);
    BITMAP *cursor = load_bitmap("cursor.bmp",NULL);
    //BITMAP *patron = load_bitmap("PatronRecorte.bmp",NULL);
    BITMAP *fondo = load_bitmap("fondo.bmp",NULL);
    FONT *MiFuente;
    FONT *TextColor;
    MiFuente =  load_font("font.pcx",NULL,NULL);
    TextColor =  load_font("textColor.pcx",NULL,NULL);

    DATAFILE *datos = load_datafile("source.dat");
    BITMAP *patron = (BITMAP*) datos[0].dat;

	HANDLE *hComm;    // using the serial port
    BOOL *Status;
    DWORD *dwEventMask;
    char TempChar;
    char SerialBuffer[256];
    DWORD *NoBytesRead;
    DCB *dcbSerialParams = {0};    // setting the parameters for the serial port
    COMMTIMEOUTS *timeouts = {0};    // setting timeouts


    int rojo = 0,verde = 0,azul = 0;
    char SerialBufferRojo[256];
    int color = 0;

    int auxXPatron = 425;
    int auxYPatron = 20;
    int xPatron = 0;
    int yPatron = 0;
    int r = 231, g = 238,b = 237;
    int i = 0;

    /*Funcionamiento principal*/
    configuracionValidacion(buffer,fondo,patron,cursor,MiFuente,&hComm,&Status,&dwEventMask,&NoBytesRead,&dcbSerialParams,&timeouts);

    while(!close_button_pressed && !key[KEY_ENTER]){

        if(mouse_b & 1){ //Tabla de colores
            xPatron = auxXPatron;
            yPatron = auxYPatron;

            //Primera Fila Colores
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 115;
                    g = 82;
                    b = 68;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 194;
                    g = 150;
                    b = 130;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 98;
                    g = 122;
                    b = 157;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 87;
                    g = 108;
                    b = 67;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 133;
                    g = 128;
                    b = 177;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 103;
                    g = 189;
                    b = 170;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 64;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 0;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 128;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 128;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 128;
                    b = 128;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 0;
                    b = 128;
            }
            xPatron += 38;


            //Segunda Fila Colores
            xPatron = auxXPatron;
            yPatron += 50 ;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 214;
                    g = 126;
                    b = 44;
            }
            xPatron += 38;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 80;
                    g = 91;
                    b = 166;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 193;
                    g = 90;
                    b = 99;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 94;
                    g = 60;
                    b = 108;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 157;
                    g = 188;
                    b = 64;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 224;
                    g = 163;
                    b = 46;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 128;
                    b = 64;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 0;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 255;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 255;
                    b = 0;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 255;
                    b = 255;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 0;
                    b = 255;
            }
            xPatron += 38;

            //Tercera Fila Colores
            xPatron = auxXPatron;
            yPatron += 50;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 56;
                    g = 61;
                    b = 150;
            }
            xPatron += 38;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 70;
                    g = 148;
                    b = 73;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 175;
                    g = 54;
                    b = 60;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 231;
                    g = 199;
                    b = 31;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 187;
                    g = 86;
                    b = 149;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 8;
                    g = 133;
                    b = 161;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 0;
                    b = 128;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 128;
                    b = 64;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 64;
                    b = 64;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 128;
                    b = 255;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 64;
                    b = 128;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 0;
                    b = 255;
            }
            xPatron += 38;

            //Cuarta Fila Colores
            xPatron = auxXPatron;
            yPatron += 50 ;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 243;
                    g = 243;
                    b = 242;
            }
            xPatron += 38;
            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 200;
                    g = 200;
                    b = 200;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 160;
                    g = 160;
                    b = 160;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 122;
                    g = 122;
                    b = 121;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 85;
                    g = 85;
                    b = 85;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 52;
                    g = 52;
                    b = 52;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 0;
                    b = 255;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 255;
                    b = 128;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 0;
                    g = 255;
                    b = 128;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 255;
                    b = 255;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 128;
                    g = 128;
                    b = 255;
            }
            xPatron += 38;

            if(mouse_x > xPatron && mouse_x < (xPatron+38) && mouse_y > yPatron && mouse_y < (yPatron + 50)){
                    r = 255;
                    g = 0;
                    b = 128;
            }
            xPatron += 38;

            //printf("X: %d Y: %d\n",mouse_x,mouse_y);//Posiciones del MOUSE
        }


	        Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);    // reading the data from the serial port

	        if(!Status){
                configuracionValidacion(buffer,fondo,patron,cursor,MiFuente,&hComm,&Status,&dwEventMask,&NoBytesRead,&dcbSerialParams,&timeouts);
	        }



	        //SerialBuffer[i] = TempChar;
	        if(TempChar != ','){
                SerialBufferRojo[i] = TempChar;
                i++;
			}
			else{
                i = 0;
                switch(color){
                    case 0:
                        rojo = atoi(SerialBufferRojo);
                        //printf("Rojo: %d ",rojo);
                        color ++;
                    break;
                    case 1:
                        verde = atoi(SerialBufferRojo);
                        //printf("Verde: %d ",verde);
                        color ++;
                    break;
                    case 2:
                        azul = atoi(SerialBufferRojo);
                        //printf("Azul: %d \n",azul);
                        color = 0;
                    break;
                }
			}


        blit(fondo,buffer,0,0,0,0,900,600);
        blit(patron,buffer,0,0,425,20,900,600);//Tabla colores
        rectfill(buffer,40,40,190,190,makecol(rojo,verde,azul));//Color del sensor
        rectfill(buffer,230,40,380,190,makecol(r,g,b));//Color seleccionado

        /*Cuadros de colores RGB Sensor y RGB Color*/

        rectfill(buffer,425,250,493,318,makecol(rojo,0,0));
        rectfill(buffer,503,250,571,318,makecol(0,verde,0));
        rectfill(buffer,581,250,649,318,makecol(0,0,azul));

        rectfill(buffer,659,250,727,318,makecol(r,0,0));
        rectfill(buffer,737,250,805,318,makecol(0,g,0));
        rectfill(buffer,815,250,883,318,makecol(0,0,b));

       /*Texto de colores RGB Sensor y RGB Cclor*/

        textprintf_ex(buffer,TextColor,450,325,makecol(0,0,0),-1,"%d",rojo);
        textprintf_ex(buffer,TextColor,528,325,makecol(0,0,0),-1,"%d",verde);
        textprintf_ex(buffer,TextColor,606,325,makecol(0,0,0),-1,"%d",azul);


        textprintf_ex(buffer,TextColor,684,325,makecol(0,0,0),-1,"%d",r);
        textprintf_ex(buffer,TextColor,762,325,makecol(0,0,0),-1,"%d",g);
        textprintf_ex(buffer,TextColor,840,325,makecol(0,0,0),-1,"%d",b);

        masked_blit(cursor,buffer,0,0,mouse_x,mouse_y,13,22);
        blit(buffer,screen,0,0,0,0,900,600);



	}

    CloseHandle(hComm);    // closing the serial port
	//Cerrar Serial Port
	destroy_bitmap(buffer);

    return 0;
}
END_OF_MAIN ()


