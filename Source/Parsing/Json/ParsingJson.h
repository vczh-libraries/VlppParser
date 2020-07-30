/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parser::ParsingJson_Parser

***********************************************************************/

#ifndef VCZH_PARSING_JSON_PARSINGJSON
#define VCZH_PARSING_JSON_PARSINGJSON

#include "ParsingJson_Parser.h"

namespace vl
{
	namespace parsing
	{
		namespace json
		{
			extern void						JsonEscapeString(const WString& text, stream::TextWriter& writer);
			extern void						JsonUnescapeString(const WString& text, stream::TextWriter& writer);

			/// <summary>Serialize JSON to string.</summary>
			/// <param name="node">The JSON node to serialize.</param>
			/// <param name="writer">The text writer to receive the string.</param>
			extern void						JsonPrint(Ptr<JsonNode> node, stream::TextWriter& writer);

			/// <summary>Serialize JSON to string.</summary>
			/// <returns>The serialized string.</returns>
			/// <param name="node">The JSON node to serialize.</param>
			extern WString					JsonToString(Ptr<JsonNode> node);
		}
	}
}

#endif