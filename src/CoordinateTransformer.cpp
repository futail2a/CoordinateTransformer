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
	//rename rot_deg
	"conf.default.rotX", "0",
	"conf.default.rotY", "0",
	"conf.default.rotZ", "0",
	// Widget
	"conf.__widget__.transX", "text",
	"conf.__widget__.transY", "text",
	"conf.__widget__.transZ", "text",
	"conf.__widget__.rotX", "text",
	"conf.__widget__.rotY", "text",
	"conf.__widget__.rotZ", "text",
	// Constraints
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
	bindParameter("rotX", m_rotX, "0");
	bindParameter("rotY", m_rotY, "0");
	bindParameter("rotZ", m_rotZ, "0");
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
	m_rotX = m_rotX* M_PI / 180.0;
	m_rotY = m_rotY* M_PI / 180.0;
	m_rotZ = m_rotZ* M_PI / 180.0;

	//各軸回転クオータニオン
	m_qX = AngleAxisd(m_rotX, Vector3d::UnitX());
	m_qY = AngleAxisd(m_rotY, Vector3d::UnitY());
	m_qZ = AngleAxisd(m_rotZ, Vector3d::UnitZ());

	//平行移動
	m_trans = Translation3d(m_transX, m_transY, m_transZ);

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

		//バッファに書き込み
		m_DestinationCoord.data.position.x = floor(m_OutputV(0) * 1000) / 1000;
		m_DestinationCoord.data.position.y = floor(m_OutputV(1) * 1000) / 1000;
		m_DestinationCoord.data.position.z = floor(m_OutputV(2) * 1000) / 1000;

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


