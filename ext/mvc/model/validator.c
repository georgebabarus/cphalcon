
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "mvc/model/validator.h"
#include "mvc/model/validatorinterface.h"
#include "mvc/model/exception.h"
#include "mvc/model/message.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/array.h"

/**
 * Phalcon\Mvc\Model\Validator
 *
 * This is a base class for Phalcon\Mvc\Model validators
 */
zend_class_entry *phalcon_mvc_model_validator_ce;

PHP_METHOD(Phalcon_Mvc_Model_Validator, __construct);
PHP_METHOD(Phalcon_Mvc_Model_Validator, appendMessage);
PHP_METHOD(Phalcon_Mvc_Model_Validator, getMessages);
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOptions);
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOption);
PHP_METHOD(Phalcon_Mvc_Model_Validator, isSetOption);

static const zend_function_entry phalcon_mvc_model_validator_method_entry[] = {
	PHP_ME(Phalcon_Mvc_Model_Validator, __construct, arginfo_phalcon_mvc_model_validator___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Mvc_Model_Validator, appendMessage, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(Phalcon_Mvc_Model_Validator, getMessages, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Mvc_Model_Validator, getOptions, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(Phalcon_Mvc_Model_Validator, getOption, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(Phalcon_Mvc_Model_Validator, isSetOption, NULL, ZEND_ACC_PROTECTED)
	PHP_FE_END
};

/**
 * Phalcon\Mvc\Model\Validator initializer
 */
PHALCON_INIT_CLASS(Phalcon_Mvc_Model_Validator){

	PHALCON_REGISTER_CLASS(Phalcon\\Mvc\\Model, Validator, mvc_model_validator, phalcon_mvc_model_validator_method_entry, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

	zend_declare_property_null(phalcon_mvc_model_validator_ce, SL("_options"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_mvc_model_validator_ce, SL("_messages"), ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;
}

/**
 * Phalcon\Mvc\Model\Validator constructor
 *
 * @param array $options
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, __construct){

	zval *options;

	phalcon_fetch_params(0, 1, 0, &options);
	
	if (Z_TYPE_P(options) != IS_ARRAY) { 
		PHALCON_THROW_EXCEPTION_STRW(phalcon_mvc_model_exception_ce, "$options argument must be an Array");
		return;
	}
	phalcon_update_property_this(this_ptr, SL("_options"), options TSRMLS_CC);
	
}

/**
 * Appends a message to the validator
 *
 * @param string $message
 * @param string $field
 * @param string $type
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, appendMessage){

	zval *message, *field = NULL, *type = NULL, *code = NULL, *class_name, *suffix;
	zval *empty_string, *model_message, *t;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 3, &message, &field, &type, &code);
	
	if (!field) {
		field = PHALCON_GLOBAL(z_null);
	}
	
	if (!type) {
		type = PHALCON_GLOBAL(z_null);
	}

	if (!code) {
		code = PHALCON_GLOBAL(z_zero);
	}
	
	if (!zend_is_true(type)) {
		PHALCON_INIT_VAR(class_name);
		phalcon_get_class(class_name, this_ptr, 0 TSRMLS_CC);
	
		PHALCON_INIT_VAR(suffix);
		ZVAL_STRING(suffix, "Validator", 1);
	
		PHALCON_INIT_VAR(empty_string);
		ZVAL_EMPTY_STRING(empty_string);
	
		PHALCON_INIT_VAR(t);
		phalcon_fast_str_replace(t, suffix, empty_string, class_name);
	}
	else {
		t = type;
	}
	
	PHALCON_INIT_VAR(model_message);
	object_init_ex(model_message, phalcon_mvc_model_message_ce);
	phalcon_call_method_p4_noret(model_message, "__construct", message, field, t, code);
	
	phalcon_update_property_array_append(this_ptr, SL("_messages"), model_message TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Returns messages generated by the validator
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getMessages){


	RETURN_MEMBER(this_ptr, "_messages");
}

/**
 * Returns all the options from the validator
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOptions){


	RETURN_MEMBER(this_ptr, "_options");
}

/**
 * Returns an option
 *
 * @param string $option
 * @return mixed
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, getOption){

	zval *option, *options, *value;

	phalcon_fetch_params(0, 1, 0, &option);
	
	options = phalcon_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (phalcon_array_isset_fetch(&value, options, option)) {
		RETURN_ZVAL(value, 1, 0);
	}
	
	RETURN_NULL();
}

/**
 * Check whether a option has been defined in the validator options
 *
 * @param string $option
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Model_Validator, isSetOption){

	zval *option, *options;

	phalcon_fetch_params(0, 1, 0, &option);
	
	options = phalcon_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	
	RETURN_BOOL(phalcon_array_isset(options, option));
}
