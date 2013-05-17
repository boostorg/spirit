auto const calculator = x3::grammar("calculator"
      
	  	expression = 
			term
			>> *(   (char_('+') >> term)
				 |   (char_('-') >> term)
				 )
		,
	  
		term = 
			factor
			>> *(   (char_('*') >> factor)
				 |   (char_('/') >> factor)
				 )
		,
	  
	  
		factor =
				 uint_
			|   '(' >> expression >> ')'
			|   (char_('-') >> factor)
			|   (char_('+') >> factor)
			;
	);
