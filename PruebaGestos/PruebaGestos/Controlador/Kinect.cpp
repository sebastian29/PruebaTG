﻿#include "Kinect.h"


Kinect::Kinect()
{
    deformacion = new Deformacion();
    visualizacion = new Visualizacion();
    gesto = NO_GESTO;
    empezarGesto = 0;
    identificador = false;
    Prueba prueba ( visualizacion );
    p = prueba;
    repeler = true;
}

Kinect::~Kinect()
{
}

void Kinect::procesarGestos()
{
    NUI_SKELETON_FRAME skeletonFrame = { 0 };
    HRESULT hr = sensor->NuiSkeletonGetNextFrame ( 0, &skeletonFrame );
    if ( FAILED ( hr ) )
    {
        return;
    }
    sensor->NuiTransformSmooth ( &skeletonFrame, NULL );
    for ( int i = 0; i < NUI_SKELETON_COUNT; ++i )
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
        if ( NUI_SKELETON_TRACKED == trackingState )
        {
            Vector4 manoDerecha = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
            Vector4 codoDerecho = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
            Vector4 manoIzquierda = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
            Vector4 codoIzquierdo = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
            Vector4 hombroCentro = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
            Vector4 cadera = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_LEFT];
            if ( gesto == NO_GESTO )
            {
                if ( ( manoDerecha.y >= codoDerecho.y ) && ( manoIzquierda.y >= codoIzquierdo.y )
                        && valoresGestos[MANOS_ARRIBA].getGestoProgreso() <= valoresGestos[MANOS_ARRIBA].getGestoCompleto() )
                {
                    valoresGestos[MANOS_ARRIBA].aumentarProgreso();
                    if ( valoresGestos[MANOS_ARRIBA].getGestoProgreso() == valoresGestos[MANOS_ARRIBA].getGestoCompleto() )
                    {
                        valoresGestos[MANOS_ARRIBA].reiniciarValores();
                        std::cout << "Empezo gesto manos arriba" << std::endl;
                        visualizacion->textoGesto ( "Trasladar" );
                        gesto = MANOS_ARRIBA;
                    }
                }
                else
                {
                    if ( ( manoDerecha.y > codoDerecho.y )
                            && valoresGestos[MANO_DERECHA_ARRIBA].getGestoProgreso() <= valoresGestos[MANO_DERECHA_ARRIBA].getGestoCompleto() )
                    {
                        valoresGestos[MANO_DERECHA_ARRIBA].aumentarProgreso();
                        if ( valoresGestos[MANO_DERECHA_ARRIBA].getGestoProgreso() == valoresGestos[MANO_DERECHA_ARRIBA].getGestoCompleto() )
                        {
                            valoresGestos[MANO_DERECHA_ARRIBA].reiniciarValores();
                            std::cout << "Empezo gesto mano derecha" << std::endl;
                            visualizacion->textoGesto ( "Deformar" );
                            visualizacion->textoAccion ( "Deformar - repeler" );
                            gesto = MANO_DERECHA_ARRIBA;
                        }
                    }
                    if ( ( manoIzquierda.y > codoIzquierdo.y )
                            && valoresGestos[MANO_IZQUIERDA_ARRIBA].getGestoProgreso() <= valoresGestos[MANO_IZQUIERDA_ARRIBA].getGestoCompleto() )
                    {
                        valoresGestos[MANO_IZQUIERDA_ARRIBA].aumentarProgreso();
                        if ( valoresGestos[MANO_IZQUIERDA_ARRIBA].getGestoProgreso() == valoresGestos[MANO_IZQUIERDA_ARRIBA].getGestoCompleto() )
                        {
                            valoresGestos[MANO_IZQUIERDA_ARRIBA].reiniciarValores();
                            visualizacion->textoGesto ( "Rotar" );
                            std::cout << "Empezo gesto mano izquierda" << std::endl;
                            gesto = MANO_IZQUIERDA_ARRIBA;
                        }
                    }
                }
            }
            else
            {
                if ( gesto == MANOS_ARRIBA )
                {
                    if ( hombroCentro.z - manoDerecha.z < 0.3 && hombroCentro.z - manoIzquierda.z < 0.3 )
                    {
                        std::cout << "Termino gesto" << std::endl;
                        reiniciarGestos();
                        visualizacion->textoAccion ( "" );
                        visualizacion->textoGesto ( "" );
                        gesto = NO_GESTO;
                    }
                    /*---------------------------------------------------------------------------------------------------------*/
                    /*MOVER DERECHA*/
                    if ( valoresGestos[MOVER_DERECHA].getValorAnterior() < redondear ( manoDerecha.x )
                            && valoresGestos[MOVER_DERECHA].getValorAnterior2() < redondear ( manoIzquierda.x )
                            && valoresGestos[MOVER_DERECHA].getGestoProgreso() <= valoresGestos[MOVER_DERECHA].getGestoCompleto()
                       )
                    {
                        valoresGestos[MOVER_DERECHA].aumentarProgreso();
                        valoresGestos[MOVER_DERECHA].setValorAnterior ( redondear ( manoDerecha.x ), redondear ( manoIzquierda.x ) );
                        if ( valoresGestos[MOVER_DERECHA].getGestoProgreso() == valoresGestos[MOVER_DERECHA].getGestoCompleto() )
                        {
                            std::cout << "mover derecha" << std::endl;
                            if ( p.realizoGesto ( MOVER_DERECHA ) )
                            {
                                visualizacion->moverHorizontal ( true );
                            }
                            reiniciarGestos();
                            visualizacion->textoAccion ( "Mover derecha" );
                        }
                    }
                    /*---------------------------------------------------------------------------------------------------------*/
                    /*MOVER IZQUIERDA*/
                    if ( valoresGestos[MOVER_IZQUIERDA].getValorAnterior() > redondear ( manoDerecha.x )
                            && valoresGestos[MOVER_IZQUIERDA].getValorAnterior2() > redondear ( manoIzquierda.x )
                            && valoresGestos[MOVER_IZQUIERDA].getGestoProgreso() <= valoresGestos[MOVER_IZQUIERDA].getGestoCompleto()
                       )
                    {
                        valoresGestos[MOVER_IZQUIERDA].aumentarProgreso();
                        valoresGestos[MOVER_IZQUIERDA].setValorAnterior ( redondear ( manoDerecha.x ), redondear ( manoIzquierda.x ) );
                        if ( valoresGestos[MOVER_IZQUIERDA].getGestoProgreso() == valoresGestos[MOVER_IZQUIERDA].getGestoCompleto() )
                        {
                            std::cout << "mover izquierda" << std::endl;
                            if ( p.realizoGesto ( MOVER_IZQUIERDA ) )
                            {
                                visualizacion->moverHorizontal ( false );
                            }
                            reiniciarGestos();
                            visualizacion->textoAccion ( "Mover izquierda" );
                        }
                    }
                    /*---------------------------------------------------------------------------------------------------------*/
                    /*MOVER ARRIBA*/
                    if ( valoresGestos[MOVER_ARRIBA].getValorAnterior() < redondear ( manoDerecha.y )
                            && valoresGestos[MOVER_ARRIBA].getValorAnterior2() < redondear ( manoIzquierda.y )
                            && valoresGestos[MOVER_ARRIBA].getGestoProgreso() <= valoresGestos[MOVER_ARRIBA].getGestoCompleto()
                       )
                    {
                        valoresGestos[MOVER_ARRIBA].aumentarProgreso();
                        valoresGestos[MOVER_ARRIBA].setValorAnterior ( redondear ( manoDerecha.y ), redondear ( manoIzquierda.y ) );
                        if ( valoresGestos[MOVER_ARRIBA].getGestoProgreso() == valoresGestos[MOVER_ARRIBA].getGestoCompleto() )
                        {
                            std::cout << "mover arriba" << std::endl;
                            if ( p.realizoGesto ( MOVER_ARRIBA ) )
                            {
                                visualizacion->moverVertical ( true );
                            }
                            reiniciarGestos();
                            visualizacion->textoAccion ( "Mover arriba" );
                        }
                    }
                    /*---------------------------------------------------------------------------------------------------------*/
                    /*MOVER ABAJO*/
                    if ( valoresGestos[MOVER_ABAJO].getValorAnterior() > redondear ( manoDerecha.y )
                            && valoresGestos[MOVER_ABAJO].getValorAnterior2() > redondear ( manoIzquierda.y )
                            && valoresGestos[MOVER_ABAJO].getGestoProgreso() <= valoresGestos[MOVER_ABAJO].getGestoCompleto()
                       )
                    {
                        valoresGestos[MOVER_ABAJO].aumentarProgreso();
                        valoresGestos[MOVER_ABAJO].setValorAnterior ( redondear ( manoDerecha.y ), redondear ( manoIzquierda.y ) );
                        if ( valoresGestos[MOVER_ABAJO].getGestoProgreso() == valoresGestos[MOVER_ABAJO].getGestoCompleto() )
                        {
                            std::cout << "mover abajo" << std::endl;
                            if ( p.realizoGesto ( MOVER_ABAJO ) )
                            {
                                visualizacion->moverVertical ( false );
                            }
                            reiniciarGestos();
                            visualizacion->textoAccion ( "Mover abajo" );
                        }
                    }
                }
                if ( gesto == MANO_IZQUIERDA_ARRIBA )
                {
                    if ( manoDerecha.y > codoDerecho.y )
                    {
                        visualizacion->textoGesto ( "Zoom" );
                        if ( hombroCentro.z - manoDerecha.z < 0.3 && hombroCentro.z - manoIzquierda.z < 0.3 )
                        {
                            std::cout << "Termino gesto" << std::endl;
                            reiniciarGestos();
                            visualizacion->textoAccion ( "" );
                            visualizacion->textoGesto ( "" );
                            gesto = NO_GESTO;
                        }
                        else
                        {
                            float dist = distancia ( manoDerecha.x, manoIzquierda.x, manoDerecha.y, manoIzquierda.y );
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ZOOM IN*/
                            if ( dist > valoresGestos[ZOOM_IN].getValorAnterior()
                                    && valoresGestos[ZOOM_IN].getGestoProgreso() <= valoresGestos[ZOOM_IN].getGestoCompleto()
                               )
                            {
                                valoresGestos[ZOOM_IN].aumentarProgreso();
                                valoresGestos[ZOOM_IN].setValorAnterior ( dist );
                                if ( valoresGestos[ZOOM_IN].getGestoProgreso() == valoresGestos[ZOOM_IN].getGestoCompleto() )
                                {
                                    std::cout << "zoom in" << std::endl;
                                    if ( p.realizoGesto ( ZOOM_IN ) )
                                    {
                                        visualizacion->zoom ( true );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Acercar" );
                                }
                            }
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ZOOM OUT*/
                            if ( dist < valoresGestos[ZOOM_OUT].getValorAnterior()
                                    && valoresGestos[ZOOM_OUT].getGestoProgreso() <= valoresGestos[ZOOM_OUT].getGestoCompleto() )
                            {
                                valoresGestos[ZOOM_OUT].aumentarProgreso();
                                valoresGestos[ZOOM_OUT].setValorAnterior ( dist );
                                if ( valoresGestos[ZOOM_OUT].getGestoProgreso() == valoresGestos[ZOOM_OUT].getGestoCompleto() )
                                {
                                    std::cout << "zoom out" << std::endl;
                                    if ( p.realizoGesto ( ZOOM_OUT ) )
                                    {
                                        visualizacion->zoom ( false );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Alejar" );
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( hombroCentro.z - manoIzquierda.z < 0.3 )
                        {
                            std::cout << "Termino gesto" << std::endl;
                            visualizacion->textoAccion ( "" );
                            visualizacion->textoGesto ( "" );
                            reiniciarGestos();
                            gesto = NO_GESTO;
                        }
                        else
                        {
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ROTAR IZQUIERDA*/
                            if ( valoresGestos[ROTAR_IZQUIERDA].getValorAnterior() > redondear ( manoIzquierda.x )
                                    && valoresGestos[ROTAR_IZQUIERDA].getGestoProgreso() <= valoresGestos[ROTAR_IZQUIERDA].getGestoCompleto() )
                            {
                                valoresGestos[ROTAR_IZQUIERDA].aumentarProgreso();
                                valoresGestos[ROTAR_IZQUIERDA].setValorAnterior ( redondear ( manoIzquierda.x ) );
                                if ( valoresGestos[ROTAR_IZQUIERDA].getGestoProgreso() == valoresGestos[ROTAR_IZQUIERDA].getGestoCompleto() )
                                {
                                    std::cout << "rotar izquierda\n";
                                    if ( p.realizoGesto ( ROTAR_IZQUIERDA ) )
                                    {
                                        visualizacion->rotarHorizontal ( false );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Rotar izquierda" );
                                }
                            }
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ROTAR DERECHA*/
                            if ( valoresGestos[ROTAR_DERECHA].getValorAnterior() < redondear ( manoIzquierda.x )
                                    && valoresGestos[ROTAR_DERECHA].getGestoProgreso() <= valoresGestos[ROTAR_DERECHA].getGestoCompleto() )
                            {
                                valoresGestos[ROTAR_DERECHA].aumentarProgreso();
                                valoresGestos[ROTAR_DERECHA].setValorAnterior ( redondear ( manoIzquierda.x ) );
                                if ( valoresGestos[ROTAR_DERECHA].getGestoProgreso() == valoresGestos[ROTAR_DERECHA].getGestoCompleto() )
                                {
                                    std::cout << "rotar derecha\n";
                                    if ( p.realizoGesto ( ROTAR_DERECHA ) )
                                    {
                                        visualizacion->rotarHorizontal ( true );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Rotar derecha" );
                                }
                            }
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ROTAR ARRIBA*/
                            if ( valoresGestos[ROTAR_ARRIBA].getValorAnterior() < redondear ( manoIzquierda.y )
                                    && valoresGestos[ROTAR_ARRIBA].getGestoProgreso() <= valoresGestos[ROTAR_ARRIBA].getGestoCompleto() )
                            {
                                valoresGestos[ROTAR_ARRIBA].aumentarProgreso();
                                valoresGestos[ROTAR_ARRIBA].setValorAnterior ( redondear ( manoIzquierda.y ) );
                                if ( valoresGestos[ROTAR_ARRIBA].getGestoProgreso() == valoresGestos[ROTAR_ARRIBA].getGestoCompleto() )
                                {
                                    std::cout << "rotar arriba\n";
                                    if ( p.realizoGesto ( ROTAR_ARRIBA ) )
                                    {
                                        visualizacion->rotarVertical ( true );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Rotar arriba" );
                                }
                            }
                            /*---------------------------------------------------------------------------------------------------------*/
                            /*ROTAR ABAJO*/
                            if ( valoresGestos[ROTAR_ABAJO].getValorAnterior() > redondear ( manoIzquierda.y )
                                    && valoresGestos[ROTAR_ABAJO].getGestoProgreso() <= valoresGestos[ROTAR_ABAJO].getGestoCompleto() )
                            {
                                valoresGestos[ROTAR_ABAJO].aumentarProgreso();
                                valoresGestos[ROTAR_ABAJO].setValorAnterior ( redondear ( manoIzquierda.y ) );
                                if ( valoresGestos[ROTAR_ABAJO].getGestoProgreso() == valoresGestos[ROTAR_ABAJO].getGestoCompleto() )
                                {
                                    std::cout << "rotar abajo\n";
                                    if ( p.realizoGesto ( ROTAR_ABAJO ) )
                                    {
                                        visualizacion->rotarVertical ( true );
                                    }
                                    reiniciarGestos();
                                    visualizacion->textoAccion ( "Rotar abajo" );
                                }
                            }
                        }
                    }
                }
                if ( gesto == MANO_DERECHA_ARRIBA )
                {
                    visualizacion->activarDeformacion ( true );
                    if ( manoIzquierda.y > codoIzquierdo.y )
                    {
                        if ( repeler )
                        {
                            repeler = false;
                            visualizacion->cambioDeformacion ( repeler );
                            visualizacion->textoAccion ( "Deformar atraer" );
                        }
                    }
                    else
                    {
                        if ( !repeler )
                        {
                            repeler = true;
                            visualizacion->cambioDeformacion ( repeler );
                            visualizacion->textoAccion ( "Deformar repeler" );
                        }
                    }
                    if ( hombroCentro.z - manoDerecha.z < 0.25 )
                    {
                        std::cout << "Termino gesto" << std::endl;
                        visualizacion->activarDeformacion ( false );
                        visualizacion->textoAccion ( "" );
                        visualizacion->textoGesto ( "" );
                        reiniciarGestos();
                        gesto = NO_GESTO;
                    }
                    convertirCoordenadas ( manoDerecha.x, manoDerecha.y );
                }
            }
            if ( manoDerecha.y < cadera.y && manoIzquierda.y < cadera.y )
            {
                std::cout << "Termino gesto" << std::endl;
                gesto = NO_GESTO;
                reiniciarGestos();
                visualizacion->textoGesto ( "" );
                visualizacion->textoAccion ( "" );
                visualizacion->activarDeformacion ( false );
            }
        }
        /*std::cout << "---------------------------------------------" << std::endl;
        std::cout << "Mano derecha: ";
        std::cout << manoDerecha.x << " ";
        std::cout << manoDerecha.y << " ";
        std::cout << manoDerecha.z << std::endl;
        std::cout << "Mano izquierda: ";
        std::cout << manoIzquierda.x << " ";
        std::cout << manoIzquierda.y << " ";
        std::cout << manoIzquierda.z << std::endl;*/
    }
}
float Kinect::distancia ( float x1, float x2, float y1, float y2 )
{
    return sqrt ( pow ( x2 - x1, 2 ) + pow ( y2 - y1, 2 ) );
}
void Kinect::asignarValoresGestos()
{
    ManejadorGestos mj;
    for ( int i = 0; i < 14; i++ )
    {
        valoresGestos.push_back ( mj );
    }
    valoresGestos[MANO_DERECHA_ARRIBA].asignarValores ( 0, 7 );
    valoresGestos[MANO_IZQUIERDA_ARRIBA].asignarValores ( 0, 7 );
    valoresGestos[ROTAR_DERECHA].asignarValores ( -2, 10 );
    valoresGestos[ROTAR_IZQUIERDA].asignarValores ( 2, 10 );
    valoresGestos[ROTAR_ARRIBA].asignarValores ( -2, 10 );
    valoresGestos[ROTAR_ABAJO].asignarValores ( 2, 10 );
    valoresGestos[MANOS_ARRIBA].asignarValores ( 0, 5 );
    valoresGestos[ZOOM_IN].asignarValores ( 0, 9 );
    valoresGestos[ZOOM_OUT].asignarValores ( 10, 9 );
    valoresGestos[MOVER_DERECHA].asignarValores ( -2, -2, 7 );
    valoresGestos[MOVER_IZQUIERDA].asignarValores ( 2, 2, 7 );
    valoresGestos[MOVER_ARRIBA].asignarValores ( 0, 0, 7 );
    valoresGestos[MOVER_ABAJO].asignarValores ( 4, 4, 7 );
}
void Kinect::reiniciarGestos()
{
    for ( int i = 0; i < 14; i++ )
    {
        valoresGestos[i].reiniciarValores();
    }
}
void Kinect::deteccion()
{
    if ( WAIT_OBJECT_0 == WaitForSingleObject ( m_hNextSkeletonEvent, 0 ) )
    {
        procesarGestos();
        // Sleep ( 3000 );
    }
}
void Kinect::inicializar()
{
    if ( !inicializarKinect() )
    {
        std::cout << "Kinect no esta conectado";
        return;
    }
    visualizacion->mostrarObjetoInicial ( deformacion->crearEsfera() );
    this->asignarValoresGestos();
    p.empezarGesto();
    while ( true )
    {
        this->deteccion();
    }
}
bool Kinect::inicializarKinect()
{
    INuiSensor * pNuiSensor;
    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount ( &iSensorCount );
    if ( FAILED ( hr ) )
    {
        return false;
    }
    for ( int i = 0; i < iSensorCount; ++i )
    {
        hr = NuiCreateSensorByIndex ( i, &pNuiSensor );
        if ( FAILED ( hr ) )
        {
            continue;
        }
        hr = pNuiSensor->NuiStatus();
        if ( S_OK == hr )
        {
            sensor = pNuiSensor;
            break;
        }
        pNuiSensor->Release();
    }
    if ( NULL != sensor )
    {
        hr = sensor->NuiInitialize ( NUI_INITIALIZE_FLAG_USES_SKELETON );
        if ( SUCCEEDED ( hr ) )
        {
            m_hNextSkeletonEvent = CreateEventW ( NULL, TRUE, FALSE, NULL );
            hr = sensor->NuiSkeletonTrackingEnable ( m_hNextSkeletonEvent, 0 );
        }
    }
    if ( NULL == sensor || FAILED ( hr ) )
    {
        std::cout << "Kinect no conectado";
        return false;
    }
    return true;
}
float Kinect::redondear ( float n )
{
    return roundf ( n * 100 ) / 100;
}
void Kinect::convertirCoordenadas ( double x, double y )
{
    x = ( ( x ) * 22 ) / 1;
    y = ( ( y ) * 11 ) / 1;
    visualizacion->ubicacionEsferaDeformacion ( x, y );
    double *d = visualizacion->puntoCercano ( x, y );
    if ( d != NULL )
    {
        if ( p.realizoGesto ( repeler ) )
        {
            visualizacion->actualizarVentana ( deformacion->deformar ( d, repeler ) );
        }
    }
}