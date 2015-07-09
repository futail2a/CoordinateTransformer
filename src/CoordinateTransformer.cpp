// -*- C++ -*-
/*!
* @file  CoordinateTransformer.cpp
* @brief ModuleDescription
* @date $Date$
*
* $Id$
*/

#include "CoordinateTransformer.h"

// Module specification
// <rtc-template block="module_spec">
static const char* coordinatetransformer_spec[] =
  {
    "implementation_id", "CoordinateTransformer",
    "type_name",         "CoordinateTransformer",
    "description",       "ModuleDescription",
    "version",           "1.0.0",
    "vendor",            "VenderName",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
	// Configuration variables
	"conf.default.transX", "0",
	"conf.default.transY", "0",
	"conf.default.transZ", "0",
	"conf.default.rot_degX", "0",
	"conf.default.rot_degY", "0",
	"conf.default.rot_degZ", "0",
	"conf.default.scaleX", "1",
	"conf.default.scaleY", "1",
	"conf.default.scaleZ", "1",
	"conf.default.mirrorXY", "false",
	"conf.default.mirrorYZ", "false",
	"conf.default.mirrorZX", "false",
	// Widget
	"conf.__widget__.transX", "text",
	"conf.__widget__.transY", "text",
	"conf.__widget__.transZ", "text",
	"conf.__widget__.rot_degX", "text",
	"conf.__widget__.rot_degY", "text",
	"conf.__widget__.rot_degZ", "text",
	"conf.__widget__.scaleX", "text",
	"conf.__widget__.scaleY", "text",
	"conf.__widget__.scaleZ", "text",
	"conf.__widget__.mirrorXY", "radio",
	"conf.__widget__.mirrorYZ", "radio",
	"conf.__widget__.mirrorZX", "radio",
	// Constraints
	"conf.__constraints__.mirrorXY","(true,false)",
	"conf.__constraints__.mirrorYZ","(true,false)",
	"conf.__constraints__.mirrorZX","(true,false)",
	""
}; 
// </rtc-template>

/*!
* @brief constructor
* @param manager Maneger Object
*/
CoordinateTransformer::CoordinateTransformer(RTC::Manager* manager)
// <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_SourceCoordIn("SourceCoord", m_SourceCoord),
    m_DestinationCoordOut("DestinationCoord", m_DestinationCoord)

// </rtc-template>
{
}

/*!
* @brief destructor
*/
CoordinateTransformer::~CoordinateTransformer()
{
}



RTC::ReturnCode_t CoordinateTransformer::onInitialize()
{
	// Registration: InPort/OutPort/Service
	// <rtc-template block="registration">
  // Set InPort buffers
  addInPort("SourceCoord", m_SourceCoordIn);
  
  // Set OutPort buffer
  addOutPort("DestinationCoord", m_DestinationCoordOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
	// </rtc-template>

	// <rtc-template block="bind_config">
	// Bind variables and configuration variable
	bindParameter("transX", m_transX, "0");
	bindParameter("transY", m_transY, "0");
	bindParameter("transZ", m_transZ, "0");
	bindParameter("rot_degX", m_rot_degX, "0");
	bindParameter("rot_degY", m_rot_degY, "0");
	bindParameter("rot_degZ", m_rot_degZ, "0");
	bindParameter("conf.default.scaleX", m_scaleX, "1");
	bindParameter("conf.default.scaleY", m_scaleY, "1");
	bindParameter("conf.default.scaleZ", m_scaleZ, "1");
	bindParameter("conf.default.mirrorXY", m_mirrorXY, "false");
	bindParameter("conf.default.mirrorYZ", m_mirrorYZ, "false");
	bindParameter("conf.default.mirrorZX", m_mirrorZX, "false");
	// </rtc-template>

	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CoordinateTransformer::onFinalize()
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onStartup(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onShutdown(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t CoordinateTransformer::onActivated(RTC::UniqueId ec_id)
{
	//initialize
	m_InputV << 0, 0, 0;
	m_OutputV << 0, 0, 0;

	//deg ⇒ rad
	m_rot_degX = m_rot_degX* M_PI / 180.0;
	m_rot_degY = m_rot_degY* M_PI / 180.0;
	m_rot_degZ = m_rot_degZ* M_PI / 180.0;

	//各軸回転クオータニオン
	m_qX = AngleAxisd(m_rot_degX, Vector3d::UnitX());
	m_qY = AngleAxisd(m_rot_degY, Vector3d::UnitY());
	m_qZ = AngleAxisd(m_rot_degZ, Vector3d::UnitZ());

	//平行移動
	m_trans = Translation3d(m_transX, m_transY, m_transZ);
	if (m_mirrorXY){
		m_scaleZ *= -1;
	}
	if (m_mirrorYZ){
		m_scaleX *= -1;
	}
	if (m_mirrorZX){
		m_scaleY *= -1;
	}

	return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t CoordinateTransformer::onDeactivated(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t CoordinateTransformer::onExecute(RTC::UniqueId ec_id)
{
	if (m_SourceCoordIn.isNew()){
		m_SourceCoordIn.read();
		m_InputV << m_SourceCoord.data.position.x, m_SourceCoord.data.position.y, m_SourceCoord.data.position.z;

		//平行移動
		m_OutputV = m_trans *  m_InputV;

		//X⇒Y⇒Z順に回転
		m_OutputV = m_qX * m_OutputV;
		m_OutputV = m_qY * m_OutputV;
		m_OutputV = m_qZ * m_OutputV;

		//各軸拡大縮小してバッファに書き込み
		m_DestinationCoord.data.position.x = m_OutputV(0) * m_scaleX;
		m_DestinationCoord.data.position.y = m_OutputV(1) * m_scaleY;
		m_DestinationCoord.data.position.z = m_OutputV(2) * m_scaleZ;

		m_DestinationCoordOut.write();
	}


	return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CoordinateTransformer::onAborting(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onError(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onReset(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onStateUpdate(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CoordinateTransformer::onRateChanged(RTC::UniqueId ec_id)
{
return RTC::RTC_OK;
}
*/



extern "C"
{

	void CoordinateTransformerInit(RTC::Manager* manager)
	{
		coil::Properties profile(coordinatetransformer_spec);
		manager->registerFactory(profile,
			RTC::Create<CoordinateTransformer>,
			RTC::Delete<CoordinateTransformer>);
	}

};


