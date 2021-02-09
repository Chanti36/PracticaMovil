/*
 * GAME SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#include "Game_Scene.hpp"
#include "Menu_Scene.hpp"
#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>

using namespace basics;
using namespace std;

namespace example
{
    // ID y ruta de las texturas que se deben cargar para esta escena. La textura con el mensaje de
    // carga está la primera para poder dibujarla cuanto antes:

    Game_Scene::Texture_Data Game_Scene::textures_data[] =
    {
        {ID(loading      ),   "game-scene/loading.png"       },
        {ID(invader1     ),   "game-scene/invader1.png"      },
        {ID(invader2     ),   "game-scene/invader2.png"      },
        {ID(invader3     ),   "game-scene/invader3.png"      },
        {ID(player       ),   "game-scene/player.png"        },
        {ID(ammo         ),   "game-scene/ammo.png"          },
        {ID(invader_ammo ),   "game-scene/ammo_invader.png"  },

        {ID(pausedbase      ),   "game-scene/pause0.png"    },
        {ID(pausedpressed   ),   "game-scene/pause.png"     },
        {ID(win             ),   "game-scene/win.png"       },
        {ID(gameover        ),   "game-scene/gameover.png"  },
        {ID(exitbase        ),   "game-scene/exit0.png"     },
        {ID(exitpressed     ),   "game-scene/exit.png"      },
        {ID(playagainbase   ),   "game-scene/playagain0.png"},
        {ID(playagainpressed),   "game-scene/playagain.png" }

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
    //Inicializar los atributos que puedan ser interesante cargar en la escena en otra ocasion(game over)

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
        invaders_speed=100;
        ammo_speed = 2000;

        player_lifes=3;

        paused  = false;
        oncanvas=false;
        gameover=false;
        win     = false;

        for(int rows=0;rows<5;rows++)
        {
            for(int columns =0;columns<7;columns++)
            {
                invaders[columns + (7 * rows)].invader_xpos = (((canvas_width*.5f)-(3*spritesize)) + (columns*spritesize));     //Posicion en X de los invaders
                invaders[columns + (7 * rows)].invader_ypos =((canvas_height-canvas_height*.15f)-(spritesize*(0.5f+rows)));     //Posicion en Y de los invaders
            }
        }


        for(int i = 0; i<35;i++)                //Establecer a todos los invaders como vivos
        {
            invaders[i].invader_alive = true;
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
                start_playing ();                                                                       // Se empieza a jugar cuando el usuario toca la pantalla por primera vez
            }
            else switch (event.id)
            {
                case ID(touch-started):                                                             // Al tocar la pantalla
                {
                    if(!finishedmenuinteraction)                                                        //Si no se ha acabado la partida
                    {
                        if (!paused)                                                                    //Si no está en pausa
                        {
                            if (*event[ID(x)].as<var::Float>() >(canvas_width - paused_base->get_width()) &&
                                *event[ID(y)].as<var::Float>() > (canvas_height -(paused_base->get_height() *2)))                   //Comprobar si al pulsar coloca el dedo sobre el menu de pausa
                            {
                                oncanvas = true;
                            }
                            else
                            {
                                touch_x = *event[ID(x)].as<var::Float>();                                                           //Si no guardar la posicion
                            }
                        }
                    }
                    else                                                                                //Si se había acabado la partida
                    {
                        if(*event[ID(x)].as< var::Float>() > (playagain_base->get_position_x()-(playagain_base->get_width()*.5f)) &&            //Si toca el boton de Reiniciar la escena
                           *event[ID(x)].as< var::Float>() < (playagain_base->get_position_x()+(playagain_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (playagain_base->get_position_y()-(playagain_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (playagain_base->get_position_y()+(playagain_base->get_height()*.5f)))
                            {
                                playagain_pressed->show();
                                playagain_base->hide();
                            }
                            else
                            {
                                playagain_pressed->hide();
                                playagain_base->show();
                            }
                        }
                        else
                        {
                            playagain_pressed->hide();
                            playagain_base->show();
                        }

                        if(*event[ID(x)].as< var::Float>() > (exit_base->get_position_x()-(exit_base->get_width()*.5f)) &&            //Si toca el boton de salir al menu
                           *event[ID(x)].as< var::Float>() < (exit_base->get_position_x()+(exit_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (exit_base->get_position_y()-(exit_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (exit_base->get_position_y()+(exit_base->get_height()*.5f)))
                            {
                                exit_pressed->show();
                                exit_base->hide();
                            }
                            else
                            {
                                exit_pressed->hide();
                                exit_base->show();
                            }
                        }
                        else
                        {
                            exit_pressed->hide();
                            exit_base->show();
                        }
                    }
                    break;
                }
                case ID(touch-moved):
                {
                    if(!finishedmenuinteraction)                                                        //Si no se ha acabado la partida
                    {
                        if(!paused)                                                                     //Si no está en pausa
                        {
                            if (!oncanvas)                                                              //Si no ha empezado el movimiento desde el boton de pausa
                            {
                                touch_x2 = *event[ID(x)].as<var::Float>();
                                distance = touch_x2 - touch_x;

                                x2 = x + distance;                                                      //Establecer las coordenadas en funcion de donde vaya tocando la pantalla para mover al jugador

                                if (x2 > canvas_width - (spritesize * .5f))                             //Establecer limites para que no se pueda salir de la pantalla
                                {
                                    x2 = canvas_width - (spritesize * .5f);
                                }
                                else if (x2 < (spritesize * .5f))
                                {
                                    x2 = (spritesize * .5f);
                                }
                            }
                        }
                    }
                    else                                                                                //Si se ha acabado la partida
                    {
                        if(*event[ID(x)].as< var::Float>() > (playagain_base->get_position_x()-(playagain_base->get_width()*.5f)) &&            //Boton de reiniciar la escena
                           *event[ID(x)].as< var::Float>() < (playagain_base->get_position_x()+(playagain_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (playagain_base->get_position_y()-(playagain_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (playagain_base->get_position_y()+(playagain_base->get_height()*.5f)))
                            {
                                playagain_pressed->show();
                                playagain_base->hide();
                            }
                            else
                            {
                                playagain_pressed->hide();
                                playagain_base->show();
                            }
                        }
                        else
                        {
                            playagain_pressed->hide();
                            playagain_base->show();
                        }

                        if(*event[ID(x)].as< var::Float>() > (exit_base->get_position_x()-(exit_base->get_width()*.5f)) &&            //Boton de salir al menu
                           *event[ID(x)].as< var::Float>() < (exit_base->get_position_x()+(exit_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (exit_base->get_position_y()-(exit_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (exit_base->get_position_y()+(exit_base->get_height()*.5f)))
                            {
                                exit_pressed->show();
                                exit_base->hide();
                            }
                            else
                            {
                                exit_pressed->hide();
                                exit_base->show();
                            }
                        }
                        else
                        {
                            exit_pressed->hide();
                            exit_base->show();
                        }
                    }

                    break;
                }

                case ID(touch-ended):                                                               // Al dejar de tocar la pantalla
                {
                    if(!finishedmenuinteraction)                                                        //Si no se ha acabado la partida
                    {
                        if (paused)                                                                     //Si estaba pausado
                        {
                            if (*event[ID(x)].as<var::Float>() >(canvas_width - paused_base->get_width()) &&
                                *event[ID(y)].as<var::Float>() > (canvas_height -(paused_base->get_height()*2)))       //Si al soltar el dedo todavía lo hace en el boton de pausa
                            {
                                paused = false;
                            }
                        }
                        else                                                                            //Si no estaba pausado
                        {
                            if (oncanvas)
                            {
                                if (*event[ID(x)].as<var::Float>() > (canvas_width - paused_base->get_width()) &&
                                    *event[ID(y)].as<var::Float>() > (canvas_height -(paused_base->get_height() *2)))   //Si al soltar el dedo todavía lo hace en el boton de pausa
                                {
                                    oncanvas = false;
                                    paused = true;
                                }
                            }
                            else                                                                        //Si no ha tocado el menu de pausa
                            {
                                x = x2;                                                                 //EStablecer la posicion

                                if (std::abs(*event[ID(x)].as<var::Float>() - touch_x) < 1)     //Si no ha desplazado el dedo por la pantalla ver si puede disparar
                                {
                                    if (canshoot)
                                    {
                                        Shoot();
                                        canshoot = false;
                                    }
                                }
                            }
                        }
                    }
                    else                                                                                //Si se había acabado la partida
                    {
                        if(*event[ID(x)].as< var::Float>() > (playagain_base->get_position_x()-(playagain_base->get_width()*.5f)) &&            //Boton de reiniciar la escena
                           *event[ID(x)].as< var::Float>() < (playagain_base->get_position_x()+(playagain_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (playagain_base->get_position_y()-(playagain_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (playagain_base->get_position_y()+(playagain_base->get_height()*.5f)))
                            {
                                director.run_scene (shared_ptr< Scene >(new Game_Scene));
                            }
                        }
                        if(*event[ID(x)].as< var::Float>() > (exit_base->get_position_x()-(exit_base->get_width()*.5f)) &&            //Boton de salir al menu
                           *event[ID(x)].as< var::Float>() < (exit_base->get_position_x()+(exit_base->get_width()*.5f)))
                        {
                            if(*event[ID(y)].as< var::Float>() > (exit_base->get_position_y()-(exit_base->get_height()*.5f)) &&
                               *event[ID(y)].as< var::Float>() < (exit_base->get_position_y()+(exit_base->get_height()*.5f)))
                            {
                                director.run_scene (shared_ptr< Scene >(new Menu_Scene));
                            }
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

                switch (state)
                {
                    case LOADING: render_loading   (*canvas); break;
                    case RUNNING: render_playfield (*canvas); break;
                    case ERROR:   break;
                }
            }
        }
    }

    void Game_Scene::render_loading (Canvas & canvas)                                                   //Cargar las texturas
    {
        Texture_2D * loading_texture = textures[ID(loading)].get ();
        if (loading_texture)
        {
            canvas.fill_rectangle                                                                       //Mientras poner la textura de carga
                    (
                            { canvas_width * .5f, canvas_height * .5f },
                            { loading_texture->get_width (), loading_texture->get_height () },
                            loading_texture
                    );
        }
    }


    void Game_Scene::render_playfield (Canvas & canvas)
    {
        if(background && state!=LOADING)
        {
            canvas.fill_rectangle({canvas_width*.5f,canvas_height*.5f}, {canvas_width*1.0f,canvas_height*1.0f}, background.get());  //Dibujar el fondo
        }


        if(player_lifes==3)                                                                             //Dibujar vidas en funcion de la variable del jugador
        {
            canvas.fill_rectangle({36,canvas_height-36},     {72,72}, life.get());
            canvas.fill_rectangle({(36*3),canvas_height-36}, {72,72}, life.get());
            canvas.fill_rectangle({(36*5),canvas_height-36}, {72,72}, life.get());
        }
        else if(player_lifes==2)
        {
            canvas.fill_rectangle({36,canvas_height-36},     {72,72}, life.get());
            canvas.fill_rectangle({(36*3),canvas_height-36}, {72,72}, life.get());

        }
        else if(player_lifes==1)
        {
            canvas.fill_rectangle({36,canvas_height-36},     {72,72}, life.get());
        }
        else if(player_lifes==0)
        {

        }

        if(paused)                                                                                      //Boton de Pausa
        {
            paused_base->hide();
            paused_presed->show();
        }
        else
        {
            paused_presed->hide();
            paused_base->show();
        }

        for (auto & sprite : sprites)                                                                   //Renderizar los sprites
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
                if(!background)                                                                         //Textura del fondo
                {
                    background = Texture_2D::create(ID(bg),context,"game-scene/background.png");
                    context->add(background);
                }

                if(!life)                                                                               //Textura de la vida
                {
                    life = Texture_2D::create(ID(lf1),context,"game-scene/player.png");
                    context->add(life);
                }

                // Se carga la siguiente textura (textures.size() indica cuántas llevamos cargadas):

                Texture_Data   & texture_data = textures_data[textures.size ()];
                Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

                // Se comprueba que la textura se haya cargado bien

                if (texture) context->add (texture);

                else state = ERROR;

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


        Sprite_Handle    player_handle          (new Sprite( textures[ID(player)        ].get()));
        Sprite_Handle    ammo_handle            (new Sprite( textures[ID(ammo)          ].get()));
        Sprite_Handle    invader_ammo_handle    (new Sprite( textures[ID(invader_ammo)  ].get()));

        Sprite_Handle    paused_base_handle     (new Sprite( textures[ID(pausedbase)        ].get()));
        Sprite_Handle    paused_pressed_handle  (new Sprite( textures[ID(pausedpressed)     ].get()));
        Sprite_Handle    win_handle             (new Sprite( textures[ID(win)               ].get()));
        Sprite_Handle    gameover_handle        (new Sprite( textures[ID(gameover)          ].get()));
        Sprite_Handle    playagain_base_handle  (new Sprite( textures[ID(playagainbase)     ].get()));
        Sprite_Handle    playagain_pressed_handle(new Sprite( textures[ID(playagainpressed) ].get()));
        Sprite_Handle    exit_base_handle       (new Sprite( textures[ID(exitbase)          ].get()));
        Sprite_Handle    exit_pressed_handle    (new Sprite( textures[ID(exitpressed)       ].get()));


        spritescale = (canvas_width*.1f)/player_handle->get_height();     //Calcular la escala a la que tienen que estar los sprites en función del tamaño de la pantalla

        player_handle->set_anchor(CENTER);                                                  //JUGADOR
        player_handle->set_position({canvas_width*0.5f,canvas_height*0.5f});
        player_handle->set_scale(spritescale);
        ammo_handle->set_anchor(CENTER);                                                    //AMMO
        ammo_handle->set_position({canvas_width*0.5f,canvas_height*0.5f});
        ammo_handle->set_scale(spritescale);
        invader_ammo_handle->set_anchor(CENTER);                                            //INVADERAMMO
        invader_ammo_handle->set_position({canvas_width*0.5f,canvas_height*0.5f});
        invader_ammo_handle->set_scale(spritescale);

        paused_base_handle->set_position    ({canvas_width-(paused_base_handle->get_width()*.5f), canvas_height-canvas_height*.1f});
        paused_pressed_handle->set_position ({canvas_width-(paused_base_handle->get_width()*.5f), canvas_height-canvas_height*.1f});
        win_handle->set_position            ({canvas_width*.5f, canvas_height*.75f});
        gameover_handle->set_position       ({canvas_width*.5f, canvas_height*.75f});
        playagain_base_handle->set_position ({canvas_width*.5f, canvas_height*.5f});
        playagain_base_handle->set_scale(.75f);
        playagain_pressed_handle->set_position     ({canvas_width*.5f, canvas_height*.5f});
        playagain_pressed_handle->set_scale(.75f);
        exit_base_handle->set_position      ({canvas_width*.5f, canvas_height*.5f - playagain_pressed_handle->get_height()});
        exit_base_handle->set_scale(.75f);
        exit_pressed_handle->set_position   ({canvas_width*.5f, canvas_height*.5f - playagain_pressed_handle->get_height()});
        exit_pressed_handle->set_scale(.75f);


        sprites.push_back(player_handle);
        sprites.push_back(ammo_handle);
        sprites.push_back(invader_ammo_handle);

        sprites.push_back(paused_base_handle);
        sprites.push_back(paused_pressed_handle);
        sprites.push_back(win_handle);
        sprites.push_back(gameover_handle);
        sprites.push_back(playagain_base_handle);
        sprites.push_back(playagain_pressed_handle);
        sprites.push_back(exit_base_handle);
        sprites.push_back(exit_pressed_handle);


        // Se guardan punteros a los sprites que se van a usar frecuentemente:
        player          = player_handle.get();
        ammo            = ammo_handle.get();
        invader_ammo    = invader_ammo_handle.get();

        paused_base     = paused_base_handle.get();
        paused_presed   = paused_pressed_handle.get();
        win_sprite = win_handle.get();
        gameover_sprite = gameover_handle.get();
        playagain_base = playagain_base_handle.get();
        playagain_pressed = playagain_pressed_handle.get();
        exit_base = exit_base_handle.get();
        exit_pressed = exit_pressed_handle.get();

        //INVADERS
        for(int rows=0;rows < 5;rows++)
        {
            for(int columns=0;columns < 7;columns++)
            {
                if(rows>2)
                {
                    invaders[(columns)+(rows*7)].invaders_Sprites = Sprite_Handle (new Sprite(textures[ID(invader1)].get()));
                }
                else if(rows>0)
                {
                    invaders[(columns)+(rows*7)].invaders_Sprites = Sprite_Handle (new Sprite(textures[ID(invader2)].get()));
                }
                else
                {
                    invaders[(columns)+(rows*7)].invaders_Sprites = Sprite_Handle (new Sprite(textures[ID(invader3)].get()));
                }

                invaders[(columns)+(rows*7)].invaders_Sprites -> set_position({invaders[(columns)+(rows*7)].invader_xpos,invaders[(columns)+(rows*7)].invader_ypos});
                invaders[(columns)+(rows*7)].invaders_Sprites -> set_scale(spritescale);

                sprites.push_back( invaders[(columns)+(rows*7)].invaders_Sprites);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    
    //Restablecer los datos si se inicia/reinicia el juego
    void Game_Scene::restart_game()
    {
        player->set_position({x2,y});
        ammo->set_position({canvas_width*0.5f,canvas_height+(spritesize*0.5f)});
        invader_ammo->set_position({canvas_width*0.5f,0.f});


        gameplay = WAITING_TO_START;
        paused  = false;
        oncanvas=false;
        gameover=false;
        win     = false;
        finishedmenuinteraction = false;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {
        for(int i = 0; i<35;i++)                                                                        //Se ocultan los invaders que mueran
        {
            if(!invaders[i].invader_alive)
            {
                invaders[i].invaders_Sprites->hide();
            }
        }
        if(!paused && !win && !gameover)                                                                //Si está jugando
        {

            // Se actualiza el estado de todos los sprites:

            for (auto &sprite : sprites) {
                sprite->update(time);
            }

            player->set_position({x2, y});                                                  //Actualizar la posición del personaje

            if (ammo->get_position_y() <= canvas_height + (spritesize *
                                                           0.5f))                                       // Se puede disparar unicamente cuando la bala ha salido de la pantalla
            {
                ammo->set_speed_y(ammo_speed);
                canshoot = false;
            }
            else
            {
                canshoot = true;
            }

            //Actualizar posicion de las balas enemigas
            if (invader_ammo->get_position_y() > 0)
            {
                invader_ammo->set_speed_y(-ammo_speed*.5f);
            }

            //INVADERS
            Collisions();
            InvaderAI();

            GameOver();
        }
    }

    void Game_Scene::Shoot()
    {
        ammo->set_position({x2,spritesize});                                                //Colocar la bala en la mira del jugador para dispararla
    }

    void Game_Scene:: InvaderAI()   // Controlar a los invaders
    {
        if(gameplay == PLAYING)                                                                         //Si el jugador ha iniciado la partida se activan
        {
            for (int i = 0; i<35;i++)                                                                   //Detectar colisiones laterales de los invaders
            {
                invaders[i].invaders_Sprites->set_speed_x(invaders_speed * invaders_dir);

                if ((invaders[i].invaders_Sprites->get_position_x() - (spritesize * .5f)) <= 0 &&
                    invaders[i].invader_alive)
                {
                    invaders_dir = 1;
                    going_down = true;
                }
                else if ((invaders[i].invaders_Sprites->get_position_x() + (spritesize * .5f)) >= canvas_width  &&
                invaders[i].invader_alive)
                {
                    invaders_dir = -1;
                    going_down = true;
                }

                if((invaders[i].invaders_Sprites->get_position_y() - (spritesize * .5f)) < (player->get_position_y()+ (spritesize * .5f))
                && invaders[i].invader_alive)                                                                                             //Fin del juego cuando llegan hasta el jugador
                {
                    gameover= true;
                }
            }

            if (going_down)                                                                             //Movimiento en Y de los invaders
            {
                for (int i = 0; i<35;i++)
                {
                    invaders[i].invaders_Sprites->set_speed_y(
                            -invaders_speed * .1f);

                    if (timer.get_elapsed_seconds() > 1)
                    {
                        going_down = false;
                    }
                }
            }

            if(timer.get_elapsed_seconds()>2)                                                           //Disparos de los invaders
            {
                if(invader_ammo->get_position_y() <= 0)
                {
                    invader_shooting = rand() % 35;

                    if(invaders[invader_shooting].invader_alive)                                        //Se pone la bala en la posicion de un invader aleatorio
                    {
                        invader_ammo->set_position(invaders[invader_shooting].invaders_Sprites->get_position());
                    }
                }
            }
        }
    }


    void Game_Scene::Collisions()
    {
        for(int i = 0; i<35;i++)                                                                        //Detectar colisiones entre los invaders y la bala del jugador
        {
            if(ammo->get_position_x() < (invaders[i].invaders_Sprites->get_position_x() + (spritesize*.5f )) &&
               ammo->get_position_x() > (invaders[i].invaders_Sprites->get_position_x() - (spritesize*.5f )))
            {
                if(ammo->get_position_y() < (invaders[i].invaders_Sprites->get_position_y() + (spritesize*.5f)) &&
                   ammo->get_position_y() > (invaders[i].invaders_Sprites->get_position_y() - (spritesize*.5f)))
                {
                    if(invaders[i].invader_alive)
                    {
                        ammo->set_position_y(canvas_height + (spritesize*.5f));
                        invaders[i].invader_alive = false;
                    }
                }
            }
        }


        if(invader_ammo->get_position_y() < player->get_position_y() + (spritesize*.5f) &&              //Detectar colisiones entre el jugador y la bala de los invaders
           invader_ammo->get_position_y() > player->get_position_y() - (spritesize*.5f))
        {
            if(invader_ammo->get_position_x() < player->get_position_x() + (spritesize*.5f) &&
               invader_ammo->get_position_x() > player->get_position_x() - (spritesize*.5f))
            {
                player_lifes--;                                                                         //Restar vida si le da
                invader_ammo->set_position_y(0);
            }
        }
    }

    void Game_Scene::GameOver()
    {
        for(int i = 0; i<35;i++)                                                                        //Comprobar que todos los enemigos han sido derrotados
        {
            if (!invaders[i].invader_alive)
            {
                defeatedinvaders++;
            }
            else
            {
                defeatedinvaders=0;
            }

            if(defeatedinvaders>=35)
            {
                win=true;
            }
        }

        if(player_lifes<=0)                                                                             //Comprobar que el jugador no ha sido derrotado
        {
            gameover=true;
        }

        if(win)                                                                                         //Si gana la partida
        {
            win_sprite->show();
            gameover_sprite->hide();

            playagain_base->show();
            exit_base->show();

            if(timer.get_elapsed_seconds()>1)
            {
                finishedmenuinteraction=true;
            }
            ammo->set_position_y(-spritesize);
            invader_ammo->set_position_y(-spritesize);
        }
        else if(gameover)                                                                               //Si pierde
        {
            win_sprite->hide();
            gameover_sprite->show();

            playagain_base->show();
            exit_base->show();

            for(int i = 0; i<35; i++)
            {
                invaders[i].invader_alive=false;
            }

            if(timer.get_elapsed_seconds()>1)
            {
                finishedmenuinteraction=true;
            }
            ammo->set_position_y(-spritesize);
            invader_ammo->set_position_y(-spritesize);
        }
        else
        {
            win_sprite->hide();
            gameover_sprite->hide();
            playagain_base->hide();
            playagain_pressed->hide();
            exit_base->hide();
            exit_pressed->hide();
        }
    }
}