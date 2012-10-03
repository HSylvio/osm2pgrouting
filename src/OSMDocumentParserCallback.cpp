/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "stdafx.h"
#include "OSMDocumentParserCallback.h"
#include "OSMDocument.h"
#include "Tag.h"
#include "Relation.h"
#include "Way.h"
#include "Node.h"


// define here, which streetstype you want to parse
// for applying this filter, compile with "DISTRICT" as flag (g++ -DRESTRICT)
//#define _FILTER if(m_pActWay->highway == "motorway" || m_pActWay->highway == "primary" || m_pActWay->highway == "secondary")

namespace osm
{
	
	
/*
<relation id="147411" version="5" timestamp="2010-01-22T17:02:14Z" uid="24299" user="james_hiebert" changeset="3684904">
    <member type="way" ref="25584788" role=""/>
    <member type="way" ref="35064036" role=""/>
    <member type="way" ref="35064035" role=""/>
    <member type="way" ref="35064037" role=""/>
    <member type="way" ref="35064038" role=""/>
    <member type="way" ref="35065746" role=""/>
    <member type="way" ref="48690404" role=""/>
    <member type="way" ref="24221632" role=""/>
    <tag k="name" v="Mt. Douglas Park Local Connector"/>
    <tag k="network" v="rcn"/>
    <tag k="route" v="bicycle"/>
    <tag k="type" v="route"/>
  </relation>
 */

	
/**
	Parser callback for OSMDocument files
*/
void OSMDocumentParserCallback::StartElement( const char *name, const char** atts )
{
	// START RELATIONS CODE
	if( strcmp(name,"member") == 0 )
	{
		// std::cout << "In member..." << std::endl;
		
		if( m_pActRelation && atts != NULL )
		{
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "ref" ) == 0 )
				{
					long long wayRefId = atol( value );
                	m_pActRelation->AddWayRef( wayRefId );
				}
			}
		}
	} 
	// END RELATIONS CODE


	if( strcmp(name,"nd") == 0 )
	{
		if( m_pActWay && atts != NULL )
		{
			const char* name = *atts++;
			const char* value = *atts++;
			if( strcmp(name,"ref")==0 )
			{
				long long nodeRefId = atol( value );
                                m_pActWay->AddNodeRef( m_rDocument.FindNode( nodeRefId ) );
                                  Node * node = m_rDocument.FindNode( nodeRefId );
                                  if(node != 0 ){
                                    node->numsOfUse+=1;
                                  }else {
                                    std::cout << "Reference nd=" << nodeRefId << " has no corresponding Node Entry (Maybe Node entry after Reference?)" << std::endl;
                                  }
			}
		}
	} 
	else if( strcmp(name,"node") == 0 )
	{
		if (atts != NULL)
		{
			long long id=-1;
			double lat=-1;
			double lon=-1;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "id" ) == 0 )
				{
					id = atol( value);
				}
				else if( strcmp( name, "lat" ) == 0 )
				{
					lat = atof( value );
				}
				else if( strcmp( name, "lon" ) == 0 )
				{
					lon = atof( value );
				}
			}
			if( id>0 ) m_rDocument.AddNode( new Node( id, lat, lon ) );
		}
	}
	// THIS IS THE RELATION CODE...
	else if( strcmp(name,"relation") == 0 )
	{
		if (atts != NULL)
		{
			long long id=-1;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "id" ) == 0 )
				{
					id = atol( value);
				}
			}
			if( id>0 ) m_pActRelation = new Relation( id );
			// std::cout<<"Starting relation: "<<id<<std::endl;

		}
	}
	// END OF THE RELATIONS CODE
	else if( strcmp(name,"tag") == 0 )
	{
		// <tag k="name" v="Pfänderweg"/>
		if (atts != NULL)
		{
			std::string k;
			std::string v;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "k" ) == 0 )
				{
					k = value;
				}
				else if( strcmp( name, "v" ) == 0 )
				{
					v = value;
				}
			}
			if( !k.empty() )
			{
				//  CHECKING OUT SOME DATA...
				// std::cout<<"k: "<<k<<", v: "<<v<<std::endl;
				// std::cout<<"m_pActWay: "<<m_rDocument.m_rConfig.m_Types.count(k)<<std::endl;
				// std::cout<<"thecount: "<<m_rDocument.m_rConfig.m_Types.count(k)<<std::endl;
				if( m_pActWay)
				if ( k.compare("name")==0 )
				{
					m_pActWay->name = v;
				}
				else if(  k.compare("oneway")==0 )
					{
	//					if (!v.compare("false") || !v.compare("no") || !v.compare("0"))
						if (!v.compare("true") || !v.compare("yes") || !v.compare("1"))
							m_pActWay->oneway = true;
	//					std::cout<<"Edge "<<m_pActWay->id<<" has oneway entry with value "<< v <<std::endl;
	//					m_pActWay->oneway = std::string(v);
					}
//					m_pActWay->oneway = true;					
//					std::cout<<"Edge "<<m_pActWay->id<<" is oneway"<<std::endl;

				//else if( m_pActWay && k.compare("highway")==0 )
				if( m_rDocument.m_rConfig.m_Types.count(k) )				
				{
					m_pActWay->type = k;
					m_pActWay->clss = v;
					
					
					if( m_rDocument.m_rConfig.m_Types.count(m_pActWay->type) && m_rDocument.m_rConfig.m_Types[m_pActWay->type]->m_Classes.count(m_pActWay->clss) ) {
						
						m_pActWay->AddTag( new Tag( k, v ) );
						// std::cout<<"Added tag: "<<k<<" "<<v<<std::endl;
					}
				}
				// START TAG FOR RELATION
				else if( m_pActRelation && m_rDocument.m_rConfig.m_Types.count(k)) 
				{
					if( m_rDocument.m_rConfig.m_Types.count(k) && m_rDocument.m_rConfig.m_Types[k]->m_Classes.count(v) ) {
						
						m_pActRelation->AddTag( new Tag( k, v ) );
						// std::cout<<"Added Relation tag: "<<k<<" "<<v<<std::endl;
					}	
					// std::cout<<"Relations tag: "<<k<<" "<<v<<std::endl;
				}
								
				if (m_pActRelation && k.compare("name")==0 )
				{
					m_pActRelation->name = v;
				}
				
				
				// END TAG FOR RELATION
			}
		}
	}
	else if( strcmp(name,"way") == 0 )
	{
		if (atts != NULL)
		{
			long long id=-1;
			bool visibility = false;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "id" ) == 0 )
				{
					id = atol( value);
				}
				else if( strcmp( name, "visible" ) == 0 )
				{
					visibility = strcmp(value,"true")==0;
				}
			}
			if( id>0 )
			{
				m_pActWay = new Way( id, visibility, id );
				
			}
		}
	}
	else if( strcmp(name,"osm") == 0 )
	{
	}
}



void OSMDocumentParserCallback::EndElement( const char* name )
{
	if( strcmp(name,"way") == 0 )
	{
		//#ifdef RESTRICT
		//_FILTER
		
		if( m_rDocument.m_rConfig.m_Types.count(m_pActWay->type) && m_rDocument.m_rConfig.m_Types[m_pActWay->type]->m_Classes.count(m_pActWay->clss) )
		{
		//#endif
		
		// Comment out the following to get more log output
		//std::cout<<"We need a way of type "<<m_pActWay->type<<" and class "<< m_pActWay->clss<<std::endl;
		
			m_rDocument.AddWay( m_pActWay );
			// std::cout << "First tag: " << m_pActWay->m_Tags.back()->key << ":" << m_pActWay->m_Tags.back()->value << std::endl;


		//#ifdef RESTRICT
		}
		else
		{
		// std::cout<<"We DON'T need a way of type "<<m_pActWay->type<<" and class "<< m_pActWay->clss<<std::endl;
			// delete m_pActWay;
		}
		//#endif

		m_pActWay = 0;
	}
	// THIS IS THE RELATION CODE...
	else if( strcmp(name,"relation") == 0 )
	{
		m_rDocument.AddRelation( m_pActRelation );
		m_pActRelation = 0;		

		// std::cout<<"Adding relation: "<<m_pActRelation->id<<std::endl;
	}
	// END OF THE RELATIONS CODE
	
	
	
}

}; // end namespace osm
