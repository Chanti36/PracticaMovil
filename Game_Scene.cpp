/*
 * INTRO SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#include "Game_Scene.hpp"

#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>

using namespace basics;
using namespace std;

namespace example
{
    // ---------------------------------------------------------------------------------------------
    // ID y ruta de las texturas que se deben cargar para esta escena. La textura con el mensaje de
    // carga está la primera para poder dibujarla cuanto antes:

    Game_Scene::Texture_Data Game_Scene::textures_data[] =
    {
        {ID(loading     ),   "game-scene/loading.png"       },
        {ID(invader1    ),   "game-scene/invader1.png"      },
        {ID(invader2    ),   "game-scene/invader2.png"      },
        {ID(invader3    ),   "game-scene/invader3.png"      },
        {ID(player      ),   "game-scene/player.png"        },
        {ID(ammo        ),   "game-scene/ammo.png"          }
    };

    // Pâra determinar el número de items en el array textures_data, se divide el tamaño en bytes
    // del array completo entre el tamaño en bytes de un item:

    unsigned Game_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data);


    Game_Scene::Game_Scene()
    {
        //resolución virtual
        canvas_width  =   720;
        canvas_height =  1280;

        // Se inicilizar algunos atributos


        spritesize = canvas_width*.1f;
        y = spritesize*.5f;

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------
    //Inicializar los atributos que puedan ser interesantes cargar en la escena en otra ocasion(game over)

    bool Game_Scene::initialize ()
    {
        state     = LOADING;
        suspended = true;
        gameplay  = UNINITIALIZED;

        touch_x  = canvas_width*.5f;
        touch_x2 = canvas_width*.5f;
        x2 = canvas_width*.5f;
        x = canvas_width*.5f;

        invaders_dir=1;
        invaders_speed=0.1f;

        for(int rows=0;rows<5;rows++)                               //Posicion en Y de los invaders
        {
            invaders_y[rows] = canvas_height-(spritesize*(0.5f+rows));
        }
        for(int columns =0;columns<7;columns++)                     //Posicion en X de los invaders
        {
            invaders_x[columns] = (((canvas_width*.5f)-(3*spritesize)) + (columns*spritesize));
        }

        for(int i =0;i<35;i++)
        {
            invader_alive[i] = true;
        }

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!suspended) switch (state)
            {
                case LOADING: load_textures  ();     break;
                case RUNNING: run_simulation (time); break;
                case ERROR:   break;
            }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        suspended = true;               // Se marca que la escena ha pasado a primer plano
    }

    void Game_Scene::resume ()
    {
        suspended = false;              // Se marca que la escena ha pasado a segundo plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)               // Se descartan los eventos cuando la escena está LOADING
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing ();           // Se empieza a jugar cuando el usuario toca la pantalla por primera vez
            }
            else switch (event.id)
            {
                case ID(touch-started):     // Al tocar la pantalla
                {
                    touch_x = *event[ID(x)].as< var::Float > ();
                    break;
                }
                case ID(touch-moved):
                {
                    touch_x2 = *event[ID(x)].as< var::Float > ();
                    distance = touch_x2 - touch_x;

                    x2 = x + distance;

                    if(x2>canvas_width-(spritesize*.5f))
                    {
                        x2 = canvas_width-(spritesize*.5f);
                    }
                    else if(x2<(spritesize*.5f))
                    {
                        x2 = (spritesize*.5f);
                    }

                    break;
                }
                case ID(touch-ended):       // Al dejar de tocar la pantalla
                {
                    x = x2;

                    if(std::abs( *event[ID(x)].as< var::Float>()    -   touch_x) <   .1f)
                    {
                        if(canshoot)
                        {
                            Shoot();
                            canshoot    =   false;
                        }
                    }

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render (Context & context)
    {
        if (!suspended)
        {
            //Crear un canvas si no hay uno ya creado

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                canvas->fill_rectangle ({ canvas_width*.5f,canvas_height*.5f },                                                 //FONDO
                                        { canvas_width*1.0f,canvas_height*1.0f },
                                        background.get());

                switch (state)
                {
                    case LOADING: render_loading   (*canvas); break;
                    case RUNNING: render_playfield (*canvas); break;
                    case ERROR:   break;
                }
            }
        }
    }

    void Game_Scene::render_loading (Canvas & canvas)
    {
        Texture_2D * loading_texture = textures[ID(loading)].get ();

        if (loading_texture)
        {
            canvas.fill_rectangle
                    (
                            { canvas_width * .5f, canvas_height * .5f },
                            { loading_texture->get_width (), loading_texture->get_height () },
                            loading_texture
                    );
        }
    }

    // Simplemente se dibujan todos los sprites que conforman la escena.

    void Game_Scene::render_playfield (Canvas & canvas)
    {
        if(background && state!=LOADING)
        {
            canvas.fill_rectangle({canvas_width*.5f,canvas_height*.5f}, {720,1820}, background.get());
        }


        for (auto & sprite : sprites)
        {
            sprite->render (canvas);
        }
    }


    // ---------------------------------------------------------------------------------------------
    //Cargar las texturas / fotograma para poder parar la carga, que se realiza una vez iniciada la escena para mostrar el mensaje de carga


    void Game_Scene::load_textures ()
    {


            //Cargar y subir al contexto gráfico las texturas

            Graphics_Context::Accessor context = director.lock_graphics_context ();


         if (textures.size () < textures_count)          // Mientras queden texturas sin cargar
        {
             if (context)
            {
                if(!background)
                {
                    background = Texture_2D::create(ID(bg),context,"game-scene/background.png");
                    context->add(background);
                }

                // Se carga la siguiente textura (textures.size() indica cuántas llevamos cargadas):

                Texture_Data   & texture_data = textures_data[textures.size ()];
                Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

                // Se comprueba que la textura se haya cargado bien

                if (texture) context->add (texture); else state = ERROR;

                //Una vez cargadas las texturas pasar a crear los sprites
            }
        }
        else
        if (timer.get_elapsed_seconds () > 1.f)         //Dar 1s de tiempo al mensaje de carga aunque las texturas se hayan cargado antes
        {
            create_sprites ();
            restart_game   ();

            state = RUNNING;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::create_sprites ()
    {
        // Se crean y configuran los sprites:


        Sprite_Handle    player_handle (new Sprite( textures[ID(player)].get()));
        Sprite_Handle    ammo_handle   (new Sprite( textures[ID(ammo)].get()));

        spritescale = (canvas_width*.1f)/player_handle->get_height();     //Calcular la escala a la que tienen que estar los sprites en función del tamaño de la pantalla



        player_handle->set_anchor(CENTER);
        player_handle->set_position({canvas_width*0.5f,canvas_height*0.5f});
        player_handle->set_scale(spritescale);
        ammo_handle->set_anchor(CENTER);
        ammo_handle->set_position({canvas_width*0.5f,canvas_height*0.5f});
        ammo_handle->set_scale(spritescale);


        sprites.push_back(player_handle);
        sprites.push_back(ammo_handle);




        for(int rows=0;rows<5;rows++)
        {
            for(int columns=0;columns<7;columns++)
            {
                if(rows>2)
                {
                    invaders[(columns+1)+((rows+1)*5)] = Sprite_Handle (new Sprite(textures[ID(invader1)].get()));
                }
                else if(rows>0)
                {
                    invaders[(columns+1)+((rows+1)*5)] = Sprite_Handle (new Sprite(textures[ID(invader2)].get()));
                }
                else
                {
                    invaders[(columns+1)+((rows+1)*5)] = Sprite_Handle (new Sprite(textures[ID(invader3)].get()));
                }



                invaders[(columns+1)+((rows+1)*5)]->set_position({invaders_x[columns],invaders_y[rows]});
                invaders[(columns+1)+((rows+1)*5)]->set_scale(spritescale);

                sprites.push_back( invaders[(columns+1)+((rows+1)*5)]);
            }
        }


        // Se guardan punteros a los sprites que se van a usar frecuentemente:
        player  = player_handle.get();
        ammo    = ammo_handle.get();

    }

    // ---------------------------------------------------------------------------------------------
    
    //Restablecer los datos si se inicia/reinicia el juego
    void Game_Scene::restart_game()
    {
        player->set_position({x2,y});
        ammo->set_position({canvas_width*0.5f,canvas_height+(spritesize*0.5f)});


        gameplay = WAITING_TO_START;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {

        // Se hace unitario el vector y se multiplica un el valor de velocidad para que el vector
        // resultante tenga exactamente esa longitud:


        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {
        // Se actualiza el estado de todos los sprites:

        for (auto & sprite : sprites)
        {
            sprite->update (time);
        }

        player->set_position({x2,y});                       //Actualizar la posición del personaje

        if(ammo->get_position_y() <= canvas_height+(spritesize*0.5f))   // Se puede disparar unicamente cuando la bala ha salido de la pantalla
        {
            ammo->set_position_y(ammo->get_position_y()+10);
            canshoot=false;
        }
        else
        {
            canshoot=true;
        }

        //INVADERS
        Collisions();
        InvaderAI();
    }


    void Game_Scene::Shoot()
    {
        ammo->set_position({x2,spritesize});           //Colocar la bala en la mira del jugador para dispararla
    }

    void Game_Scene:: InvaderAI()   // Controlar a los invaders
    {

/*        invaders[0]->set_speed_x(invaders_speed*invaders_dir);*/
/*

        //MOVIMIENTO
        for(int i =0;i < 35;i++)
        {

        }*/



    }



    void Game_Scene::Collisions()
    {
        for(int i =0; i<35; i++)
        {/*
            if(ammo->get_position_x() < (invaders_Sprites[i]->get_position_x() + (spritesize*.5f )) &&      //Si pueden colisionar por columna
               ammo->get_position_x() > (invaders_Sprites[i]->get_position_x() - (spritesize*.5f )))
            {
            if(ammo->get_position_y() < (invaders_Sprites[i]->get_position_y() + (spritesize*.5f)) &&       //Si colisiona en la y
               ammo->get_position_y() > (invaders_Sprites[i]->get_position_y() - (spritesize*.5f)))
            {
                        *//*ammo->set_position_y(canvas_height + (spritesize*.5f));*//*
            }
            }*/

        }

    }
}
