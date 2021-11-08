
create table sending_holiday_calendar (
	id integer: PRIMARY KEY,
	created_by integer,
	created_by_name text,
	sending_holiday_calendar_id integer,
	holiday_name text,
	org_id integer: TENANT_ID
);
